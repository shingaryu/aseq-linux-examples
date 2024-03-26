#include <stdio.h>
#include <windows.h>

#include <process.h>
#include <time.h>
#include <limits.h>

#include "libspectr.h"

#define TEST_COUNT 3
#define EXPOSURE_MULTIPLIER 10000
#define FLASH_OFFSET 0
#define FLASH_NUM_OF_BYTES_TO_READ 100
#define FRAMES_REQUIRED 11

typedef struct {
    uint16_t index;
    HANDLE hEventThreadInfoCopied;
    uint32_t exposure;
    HANDLE hStdoutMutex;
    uint32_t flashOffset;
    uint32_t flashBytesToRead;
    uint32_t flashBytesToWrite;
    uintptr_t pDeviceContext;
    HANDLE hMeasureMutex;
    HANDLE hEventMeasureAllowed;
    int *counter;
    clock_t* timerStart;
    clock_t* timerStop;
} ThreadInfo_t;

void mutexedPrint(const ThreadInfo_t* const info, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    WaitForSingleObject(info->hStdoutMutex, INFINITE);
    vprintf(format, args);
    ReleaseMutex(info->hStdoutMutex);

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

    WaitForSingleObject(info->hStdoutMutex, INFINITE);
    printf("readFlashExample from thread %d (offset: %X, bytesToRead: %d): ", info->index, info->flashOffset, info->flashBytesToRead);
    for (index = 0; index < info->flashBytesToRead; ++index) {
        printf("%d ", (uint16_t)(*(buffer + index)));
    }
    printf("\n");
    ReleaseMutex(info->hStdoutMutex);
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

void exampleStartReadFlash(ThreadInfo_t* const info)
{
    uint8_t* buffer = NULL;
    int result = 0;
    uint32_t bufferSize = 0;

    if (info == NULL) {
        printf("readFlashExample on device %d: no thread info!\n", info->index);
        return;
    }

    bufferSize = (info->flashBytesToRead > info->flashBytesToWrite) ? info->flashBytesToRead : info->flashBytesToWrite;
    buffer = (uint8_t*)calloc(bufferSize, sizeof(uint8_t));

    result = eraseFlash(&(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint(info, "readFlashExample on device %d: eraseFlash() failed with code %d\n", info->index, result);
        free(buffer);
        return;
    }

    result = readFlash(buffer, info->flashOffset, info->flashBytesToRead, &(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint(info, "readFlashExample on device %d: readFlash() failed with code %d\n", info->index, result);
        free(buffer);
        return;
    }

    mutexedPrintFlashBuffer(info, buffer);

    randomizeBuffer(buffer, bufferSize);

    result = writeFlash(buffer, info->flashOffset, info->flashBytesToWrite, &(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint(info, "readFlashExample on device %d: writeFlash() failed with code %d\n", info->index, result);
        free(buffer);
        return;
    }

    memset(buffer, 0, bufferSize);
    result = readFlash(buffer, info->flashOffset, info->flashBytesToRead, &(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint(info, "readFlashExample on device %d: readFlash() failed after a writing operation with code %d\n", info->index, result);
        free(buffer);
        return;
    }

    mutexedPrintFlashBuffer(info, buffer);
    free(buffer);
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


    WaitForSingleObject(info->hStdoutMutex, INFINITE);
    printf("exampleReadFrames from thread %d (numOfPixelsInFrame: %d): ", info->index, numOfPixelsInFrame);
    for (index = 0; index < numOfPixelsInFrame; ++index) {
        printf("%d ", (uint16_t)(*(frameBuffer + index)));
    }
    printf("\n\n");
    ReleaseMutex(info->hStdoutMutex);
}

void exampleReadFrames(ThreadInfo_t* info)
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

    clock_t localStart, localStop;

    if (info == NULL) {
        printf("readFramesExample: no thread info!\n");
        return;
    }

    result = clearMemory(&(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint(info, "readFramesExample on device %d: clearMemory() failed with code %d\n", info->index, result);
        return;
    }

    result = setAcquisitionParameters(numOfScans, numOfBlankScans, scanMode, info->exposure, &(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint(info, "readFramesExample on device %d: setAcquisitionParameters() failed with code %d\n", info->index, result);
        return;
    }

    result = getFrameFormat(NULL, NULL, NULL, &numOfPixelsInFrame, &(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint(info, "readFramesExample on device %d: getFrameFormat() failed with code %d\n", info->index, result);
        return;
    }

    result = triggerAcquisition(&(info->pDeviceContext));
    if (result != OK) {
        mutexedPrint(info, "readFramesExample on device %d: triggerAcquisition() failed with code %d\n", info->index, result);
        return;
    }

    mutexedPrint(info, "readFramesExample on device %d: numOfPixelsInFrame: %d\n", info->index, numOfPixelsInFrame);

    frameBuffer = (uint16_t*)calloc(numOfPixelsInFrame, sizeof(uint16_t));

    while (framesCollected < FRAMES_REQUIRED) {
        result = getStatus(&statusFlags, &framesInMemory, &(info->pDeviceContext));

        if (result != OK) {
            mutexedPrint(info, "readFramesExample on device %d: getStatus() failed with code %d\n", info->index, result);
            free(frameBuffer);
            return;
        }

        if (statusFlags & 2) {
            mutexedPrint(info, "readFramesExample on device %d: memory is full!\n", info->index);
            result = clearMemory(&(info->pDeviceContext));
            if (result != OK) {
                mutexedPrint(info, "readFramesExample on device %d: getStatus() failed with code %d\n", info->index, result);
                free(frameBuffer);
                return;
            }

            result = triggerAcquisition(&(info->pDeviceContext));
            if (result != OK) {
                mutexedPrint(info, "readFramesExample on device %d: triggerAcquisition() failed with code %d\n", info->index, result);
                free(frameBuffer);
                return;
            }
            continue;
        }

        if (!framesInMemory) {
            Sleep(1);
            continue;
        }

        if (framesInMemory) {
            if (framesCollected == FRAMES_REQUIRED - 1) {
                WaitForSingleObject(info->hMeasureMutex, INFINITE);
                --(*info->counter);
                if (*info->counter == 0) {
                    SetEvent(info->hEventMeasureAllowed);
                }
                ReleaseMutex(info->hMeasureMutex);

                WaitForSingleObject(info->hEventMeasureAllowed, INFINITE);

                *info->timerStart = clock();
            }

            localStart = clock();

            result = getFrame(frameBuffer, 0xFFFF, &(info->pDeviceContext));

            localStop = clock();

            if (framesCollected == FRAMES_REQUIRED - 1) {
                *info->timerStop = clock();
            }

            printf("local getFrame execution time on device %d in seconds: %f\n", info->index, (double)(localStop - localStart) / CLOCKS_PER_SEC);

            if (result != OK) {
                mutexedPrint(info, "readFramesExample on device %d: getFrame() failed with code %d\n", info->index, result);
                free(frameBuffer);
                return;
            }

            //mutexedPrintFrameBuffer(info, frameBuffer, numOfPixelsInFrame);

            ++framesCollected;
        }
    }

    mutexedPrintFrameBuffer(info, frameBuffer, numOfPixelsInFrame);
    free(frameBuffer);
}

unsigned __stdcall operateDevice(void * data)
{
    ThreadInfo_t info;
    int result = 0;

    if (data == NULL) {
        printf("operateDevice: No thread data!\n");
        return 1;
    }

    memset(&info, 0, sizeof(ThreadInfo_t));
    info = *(ThreadInfo_t*)data;
    SetEvent(info.hEventThreadInfoCopied);

    info.hStdoutMutex = CreateMutex(NULL, FALSE, "threadStdoutMutex");
    if (info.hStdoutMutex == NULL) {
        printf("Thread %d. Could not get stdout mutex!\n", info.index);
        return 2;
    }

    info.hMeasureMutex = CreateMutex(NULL, FALSE, "measureMutex");
    if (info.hMeasureMutex == NULL) {
        printf("Thread %d. Could not get measure mutex!\n", info.index);
        return 2;
    }


    mutexedPrint(&info, "Event set in thread with index %d\n", info.index);

    info.pDeviceContext = 0;
    result = connectToDeviceByIndex(info.index, &(info.pDeviceContext));
    if (result != OK) {
        mutexedPrint(&info, "Connection to device with index %d, failed with error: %d\n", info.index, result);
        CloseHandle(info.hStdoutMutex);
        free((char*)info.pDeviceContext);
        return result;
    }

    mutexedPrint(&info, "Successfully connected to device in thread with index %d\n", info.index);

    srand((unsigned int)(time(NULL)));
    exampleStartReadFlash(&info);

    exampleReadFrames(&info);

    //detachDevice();
    disconnectDeviceContext(&info.pDeviceContext);
    CloseHandle(info.hStdoutMutex);
    return 0;
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

void initializeDeviceThreadParameters(ThreadInfo_t* const info, int index, int* counter, clock_t* timerStart, clock_t* timerStop)
{
    info->index = index;
    info->flashOffset = FLASH_OFFSET;
    info->flashBytesToRead = FLASH_NUM_OF_BYTES_TO_READ;
    info->flashBytesToWrite = info->flashBytesToRead;
    info->exposure = EXPOSURE_MULTIPLIER * (index + 1);
    info->counter = counter;
    info->timerStart = timerStart;
    info->timerStop = timerStop;
}

int main(void)
{
    ThreadInfo_t threadDeviceInfo;
    HANDLE *pCurrentThread = NULL;
    HANDLE *threads = NULL;
    DWORD result = 0;
    uint16_t index = 0;

    int count = getDevicesCount(), counter = count;

    clock_t *timerStarts = NULL, *minStart = NULL;
    clock_t *timerStops = NULL, *maxStop = NULL;
    double total_t = 0;

    memset(&threadDeviceInfo, 0, sizeof(ThreadInfo_t));
    threads = calloc(count, sizeof(HANDLE));
    timerStarts = calloc(count, sizeof(clock_t));
    timerStops = calloc(count, sizeof(clock_t));

    printf("Number of devices: %d\n", count);
    exampleGetAndClearDevicesInfo();

    threadDeviceInfo.hEventThreadInfoCopied = CreateEvent(NULL, TRUE, FALSE, NULL);
    threadDeviceInfo.hEventMeasureAllowed = CreateEvent(NULL, TRUE, FALSE, NULL);

    for (index = 0; index < count; ++index) {
        pCurrentThread = threads + index;
        printf("Starting thread for device with index %d\n", index);
        initializeDeviceThreadParameters(&threadDeviceInfo, index, &counter, &timerStarts[0] + index, &timerStops[0] + index);

        *pCurrentThread = (HANDLE)_beginthreadex(NULL, 0, operateDevice, &threadDeviceInfo, 0, NULL);

        WaitForSingleObject(threadDeviceInfo.hEventThreadInfoCopied, INFINITE);
        ResetEvent(threadDeviceInfo.hEventThreadInfoCopied);
        Sleep(2);
    }

    result = WaitForMultipleObjects(count, threads, 1, INFINITE);
    printf("\nWaitForMultipleObjects result: %d\n", result);

    result = GetLastError();
    printf("GetLastError result: %d\n", result);

    for (index = 0; index < count; ++index) {
        pCurrentThread = threads + index;
        printf("Closing handle %d: %d\n", index, CloseHandle(*pCurrentThread));
    }

    minStart = &timerStarts[0];
    maxStop = &timerStops[0];

    for (index = 0; index < count; ++index) {
        total_t = (double)(*(timerStops + index) - *(timerStarts + index)) / CLOCKS_PER_SEC;
        printf("Device %d: getFrame execution time in seconds: %f\n", index, total_t);

        if (*(timerStarts + index) < *(minStart)) {
            minStart = timerStarts + index;
        }

        if (*(timerStops + index) > *(maxStop)) {
            maxStop = timerStops + index;
        }
    }

    total_t = (double)(*maxStop - *minStart) / CLOCKS_PER_SEC;
    printf("simultaneous getFrame execution time on all devices in seconds: %f\n", total_t);

    free(threads);
    free(timerStarts);
    free(timerStops);

    getchar();

    return 0;
}

