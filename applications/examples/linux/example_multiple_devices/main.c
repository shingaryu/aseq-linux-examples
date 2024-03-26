#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <stdarg.h>

#include "libspectrometer.h"

#define TEST_COUNT 2
#define EXPOSURE_MULTIPLIER 10000
#define FLASH_OFFSET 0
#define FLASH_NUM_OF_BYTES_TO_READ 100
#define FRAMES_REQUIRED 10


typedef struct {
	uint16_t index;
	uint32_t exposure;
	uint32_t flashOffset;
	uint32_t flashBytesToRead;
    uint32_t flashBytesToWrite;

	uintptr_t pDeviceContext;

} ThreadInfo_t;

static pthread_mutex_t stdoutMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t threadContextInitializedMutex = PTHREAD_MUTEX_INITIALIZER;
//static pthread_cond_t  threadContextInitializedCondition  = PTHREAD_COND_INITIALIZER;

void mutexedPrint(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&stdoutMutex);
    vprintf(format, args);
    pthread_mutex_unlock(&stdoutMutex);

    va_end(args);
}
void mutexedPrintFlashBuffer(const ThreadInfo_t* const info, const uint8_t* const buffer)
{
    uint32_t index = 0;

    if (info == NULL) {
        printf("printFlashBuffer: no thread info!\n");
        return;
    }

    if (buffer == NULL) {
        printf("printFlashBuffer on device %d: flash buffer is not initialized!\n", info->index);
        return;
    }

    pthread_mutex_lock(&stdoutMutex);
    printf("readFlashExample from thread %d (offset: %X, bytesToRead: %d): ", info->index, info->flashOffset, info->flashBytesToRead);
    for (index = 0; index < info->flashBytesToRead; ++index) {
        printf("%d ", (uint16_t)(*(buffer + index)));
    }
    printf("\n");
    pthread_mutex_unlock(&stdoutMutex);
}
void mutexedPrintFrameBuffer(const ThreadInfo_t* const info, const uint16_t* const frameBuffer, uint32_t numOfPixelsInFrame)
{
    uint32_t index = 0;

    if (info == NULL) {
        printf("mutexedPrintFrameBuffer: no thread info!\n");
        return;
    }

    if (frameBuffer == NULL) {
        printf("mutexedPrintFrameBuffer on device %d: frame buffer is not initialized!\n", info->index);
        return;
    }

    if (numOfPixelsInFrame == 0) {
        printf("mutexedPrintFrameBuffer on device %d: zero pixels size!\n", info->index);
        return;
    }

    pthread_mutex_lock(&stdoutMutex);
    printf("exampleReadFrames from thread %d (numOfPixelsInFrame: %d): ", info->index, numOfPixelsInFrame);
    for (index = 0; index < numOfPixelsInFrame; ++index) {
        printf("%d ", (uint16_t)(*(frameBuffer + index)));
    }
    printf("\n\n");
    pthread_mutex_unlock(&stdoutMutex);
}


void exampleGetAndClearDevicesInfo();
void initializeDeviceThreadParameters(ThreadInfo_t* const, int);
void *operateDevice(void *);
void randomizeBuffer(uint8_t*, uint32_t);
void exampleReadFlash(ThreadInfo_t* const);
void exampleReadFrames(ThreadInfo_t* const);


void clearGlobals()
{
    pthread_mutex_destroy(&stdoutMutex);
    pthread_mutex_destroy(&threadContextInitializedMutex);
}

int main(void)
{
    ThreadInfo_t threadDeviceInfo;
    pthread_t *threads;
    pthread_t *pCurrentThread;

    uint16_t index = 0;

    int count = getDevicesCount();
    printf("Number of devices: %d\n", count);

    if (!count) {
        printf("No devices found. Exiting...\n");
        clearGlobals();
        return EXIT_SUCCESS;
    }

    exampleGetAndClearDevicesInfo();

    memset(&threadDeviceInfo, 0, sizeof(ThreadInfo_t));
    threads = calloc(count, sizeof(pthread_t));

    for (index = 0; index < count; ++index) {
        pthread_mutex_lock(&threadContextInitializedMutex);
        pCurrentThread = threads + index;
        printf("Starting thread for device with index %d\n", index);
        initializeDeviceThreadParameters(&threadDeviceInfo, index);
        pthread_create(pCurrentThread, NULL, operateDevice, &threadDeviceInfo);

        sleep(1);
    }

    for (index = 0; index < count; ++index) {
        pCurrentThread = threads + index;
        pthread_join(*pCurrentThread, NULL);
    }

    free(threads);
    clearGlobals();

    return EXIT_SUCCESS;
}


void exampleGetAndClearDevicesInfo()
{
    DeviceInfo_t *devices = getDevicesInfo();
    DeviceInfo_t *current = devices;

    uint16_t index = 0;

    while (current != NULL) {
        printf("HID device serial: %s (index: %d)\n", current->serialNumber, index++);
        current = current->next;
    }

    clearDevicesInfo(devices);
}

void initializeDeviceThreadParameters(ThreadInfo_t* const info, int index)
{
    info->index = index;
    info->flashOffset = FLASH_OFFSET;
    info->flashBytesToRead = FLASH_NUM_OF_BYTES_TO_READ;
    info->flashBytesToWrite = info->flashBytesToRead;
    info->exposure = EXPOSURE_MULTIPLIER * (index + 1);
}

void * operateDevice(void * data)
{
    ThreadInfo_t info;
    int result = 0;

    if (data == NULL) {
        printf("operateDevice: No thread data!\n");
        pthread_exit(0);
    }

    memset(&info, 0, sizeof(ThreadInfo_t));
    info = *(ThreadInfo_t*)data;

    pthread_mutex_unlock(&threadContextInitializedMutex);

    //mutexedPrint("Condition signalled in thread with index %d\n", info.index);

    info.pDeviceContext = 0;
    result = connectToDeviceByIndex(info.index, &(info.pDeviceContext));
    if (result != OK) {
        mutexedPrint("Connection to device with index %d, failed with error: %d\n", info.index, result);
        free((char*)info.pDeviceContext);
        pthread_exit(0);
    }

    mutexedPrint("Successfully connected to device in thread with index %d\n", info.index);

    srand((unsigned int)(time(NULL)));
    exampleReadFlash(&info);

    exampleReadFrames(&info);

    //detachDevice();
    disconnectDeviceContext(&info.pDeviceContext);
    pthread_exit(0);
}

void randomizeBuffer(uint8_t* buffer, uint32_t size)
{
    uint32_t index = 0;

    if (buffer == NULL) {
        printf("randomizeBuffer: buffer is not initialized!\n");
        return;
    }

    if (size == 0) {
        printf("randomizeBuffer: zero buffer size!\n");
        return;
    }

    for (index = 0; index < size; ++index) {
        *(buffer + index) = rand() % (UCHAR_MAX + 1);
    }
}

void exampleReadFlash(ThreadInfo_t* const info)
{
    uint8_t* buffer = NULL;
    int result = OK;
    uint32_t bufferSize = 0;

    if (info == NULL) {
        printf("exampleFlashOperations on device %d: no thread info!\n", info->index);
        return;
    }

    bufferSize = (info->flashBytesToRead > info->flashBytesToWrite) ? info->flashBytesToRead : info->flashBytesToWrite;
    buffer = (uint8_t*)calloc(bufferSize, sizeof(uint8_t));

    mutexedPrint("exampleFlashOperations on device %d: erasing flash... ", info->index);
    result = eraseFlash(&(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint("failed with code %d\n", result);
        free(buffer);
        return;
    }
    mutexedPrint("success!\n");

    mutexedPrint("exampleFlashOperations on device %d: reading %d bytes of flash memory from offset %d... ", info->index, info->flashBytesToRead, info->flashOffset);
    result = readFlash(buffer, info->flashOffset, info->flashBytesToRead, &(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint("failed with code %d\n", result);
        free(buffer);
        return;
    }
    mutexedPrint("success!\n");

    mutexedPrintFlashBuffer(info, buffer);

    randomizeBuffer(buffer, bufferSize);

    mutexedPrint("exampleFlashOperations on device %d: writing %d random values to flash memory from offset %d... ", info->index, info->flashBytesToWrite, info->flashOffset);
    result = writeFlash(buffer, info->flashOffset, info->flashBytesToWrite, &(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint("failed with code %d\n", result);
        free(buffer);
        return;
    }
    mutexedPrint("success!\n");

    memset(buffer, 0, bufferSize);

    mutexedPrint("exampleFlashOperations on device %d: reading %d bytes of flash memory from offset %d... ", info->index, info->flashBytesToRead, info->flashOffset);
    result = readFlash(buffer, info->flashOffset, info->flashBytesToRead, &(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint("failed with code %d\n", result);
        free(buffer);
        return;
    }
    mutexedPrint("success!\n");

    mutexedPrintFlashBuffer(info, buffer);
    free(buffer);
}

void exampleReadFrames(ThreadInfo_t* const info)
{
    int result = 0;
    uint16_t numOfScans = 1;
    const uint16_t numOfBlankScans = 0;
    const uint8_t scanMode = 3;
    uint16_t numOfPixelsInFrame = 0;
    uint16_t* frameBuffer = NULL;
    uint8_t  statusFlags = 0;
    uint16_t framesInMemory = 0;
    uint8_t framesCollected = 0;

    if (info == NULL) {
        printf("exampleReadFrames: no thread info!\n");
        return;
    }

    mutexedPrint("exampleReadFrames on device %d: clearing memory... ", info->index);
    result = clearMemory(&(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint("failed with code %d\n", info->index, result);
        return;
    }
    mutexedPrint("success!\n");

    mutexedPrint("exampleReadFrames on device %d: setting default acquisition parameters (numOfScans = %d, numOfBlankScans = %d, scanMode = %d, exposure = %d)... ",
                                                                                         info->index, numOfScans, numOfBlankScans, scanMode, info->exposure);
    result = setAcquisitionParameters(numOfScans, numOfBlankScans, scanMode, info->exposure, &(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint("failed with code %d\n", result);
        return;
    }
    mutexedPrint("success!\n");

    mutexedPrint("exampleReadFrames on device %d: getting frame format parameters... ", info->index);
    result = getFrameFormat(NULL, NULL, NULL, &numOfPixelsInFrame, &(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint("failed with code %d\n", result);
        return;
    }
    mutexedPrint("success!\n");
    mutexedPrint("exampleReadFrames on device %d: numOfPixelsInFrame: %d\n", info->index, numOfPixelsInFrame);

    mutexedPrint("exampleReadFrames on device %d: triggering acquisition... ", info->index);
    result = triggerAcquisition(&(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint("failed with code %d\n", info->index);
        return;
    }
    mutexedPrint("success!\n");

    frameBuffer = (uint16_t*)calloc(numOfPixelsInFrame, sizeof(uint16_t));

    while (framesCollected < FRAMES_REQUIRED + 1) {
        mutexedPrint("exampleReadFrames on device %d: starting to collect frame no. %d...\n", info->index, framesCollected);


        mutexedPrint("exampleReadFrames on device %d: getting device status...", info->index);
        result = getStatus(&statusFlags, &framesInMemory, &(info->pDeviceContext));
        if (result != OK) {
            mutexedPrint("failed with code %d\n", result);
            free(frameBuffer);
            return;
        }
        mutexedPrint("success! Status flags (hex): %x\n", statusFlags);

        if (statusFlags & 2) {
            mutexedPrint("exampleReadFrames on device %d: memory is full!\n", info->index);

            mutexedPrint("exampleReadFrames on device %d: clearing memory... ", info->index);
            result = clearMemory(&(info->pDeviceContext));
            if (result != OK) {
                mutexedPrint("failed with code %d\n", result);
                free(frameBuffer);
                return;
            }
            mutexedPrint("success!\n");

            mutexedPrint("exampleReadFrames on device %d: triggering acquisition... ", info->index);
            result = triggerAcquisition(&(info->pDeviceContext));
            if (result != OK) {
                mutexedPrint("exampleReadFrames on device %d: triggerAcquisition() failed with code %d\n", info->index, result);
                free(frameBuffer);
                return;
            }
            mutexedPrint("success!\n");
            continue;
        }

        if (!framesInMemory) {
            sleep(1);
            continue;
        }

        if (framesInMemory) {
            mutexedPrint("exampleReadFrames on device %d: reading frame no. %d...", info->index, framesCollected);
            result = getFrame(frameBuffer, 0xFFFF, &(info->pDeviceContext));

            if (result != OK) {
                mutexedPrint("failed with code %d\n", result);
                free(frameBuffer);
                return;
            }
            mutexedPrint("success!\n");

            //mutexedPrintFrameBuffer(info, frameBuffer, numOfPixelsInFrame);

            ++framesCollected;
        }
    }

    mutexedPrint("Printing last received frame. ");
    mutexedPrintFrameBuffer(info, frameBuffer, numOfPixelsInFrame);
    free(frameBuffer);
}
