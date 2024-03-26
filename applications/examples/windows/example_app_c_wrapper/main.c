#include <stdio.h>
#include <windows.h>
#include <limits.h>
#include <time.h>

#include "spectrlib_wrapper.h"

#define EXPOSURE_MULTIPLIER 10000
#define READ_FLASH_OFFSET 0
#define READ_FLASH_BYTES 50
#define WRITE_FLASH_OFFSET 0
#define WRITE_FLASH_BYTES 50
#define NUM_OF_FRAMES_TO_READ 100
#define FRAMES_REQUIRED 11

int devicesCount = 0;
uint16_t** frameBuffers;
uint8_t** flashBuffers;

uint16_t *numOfPixelsInFrame;
int *results;
int *statuses;
int *errors;

void initGlobals();
void clearGlobals();
void waitForOperationsFinish(int requiredOperation);
void printErrors();

void printFlashBuffers();
void printFlashBuffer(int deviceIndex);
void printFrameBuffers();
void printFrameBuffer(int deviceIndex);

void exampleConnectDevices();
void exampleStartReadFlash();
void exampleStartWriteFlash();
void exampleSetAndStartAcquisition();
void exampleStartGetFrame();


void initGlobals()
{
    int i = 0;

    frameBuffers = calloc(devicesCount, sizeof(uint16_t*));
    flashBuffers = calloc(devicesCount, sizeof(uint8_t*));

    for (i = 0; i < devicesCount; ++i) {
        flashBuffers[i] = calloc(READ_FLASH_BYTES, sizeof(uint8_t));
    }

    results = calloc(devicesCount, sizeof(int));
    memset(results, OK, sizeof(int) * devicesCount);
    statuses = calloc(devicesCount, sizeof(int));
    memset(statuses, IDLE, sizeof(int) * devicesCount);

    errors = (int*)calloc(devicesCount, sizeof(int));

    numOfPixelsInFrame = calloc(devicesCount, sizeof(uint16_t));
}
void clearGlobals()
{
    uint8_t index = 0;

    free(numOfPixelsInFrame);
    numOfPixelsInFrame = NULL;

    free(statuses);
    statuses = NULL;

    free(results);
    results = NULL;

    for (index = 0; index < devicesCount; ++index) {
        free(frameBuffers[index]);
        frameBuffers[index] = NULL;

        free(flashBuffers[index]);
        flashBuffers[index] = NULL;
    }

    free(frameBuffers);
    frameBuffers = NULL;
    free(flashBuffers);
    flashBuffers = NULL;

    free(errors);
    errors = NULL;
}
void waitForOperationsFinish(int requiredOperation)
{
    uint8_t devicesReady = 0, deviceIndex = 0;
    int *operationCodes;
    char idle[] = "IDLE";
    char pending[] = "PENDING";
    char finished[] = "FINISHED";
    char* current = NULL;

    memset(statuses, IDLE, sizeof(int) * devicesCount);

    operationCodes = (int*)calloc(devicesCount, sizeof(int));
    memset(operationCodes, NO_OPERATION, devicesCount * sizeof(int));

    for (deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
        if (results[deviceIndex] != OK) {
            ++devicesReady;
            operationCodes[deviceIndex] = requiredOperation;
            statuses[deviceIndex] = FINISHED;
        }
    }

    while (devicesReady != devicesCount) {
        for (deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
            if ((operationCodes[deviceIndex] == requiredOperation) && (statuses[deviceIndex] == FINISHED)) {
                continue;
            }

            results[deviceIndex] = getCurrentOperationInfo(deviceIndex, &operationCodes[deviceIndex], &statuses[deviceIndex]);

            if ((results[deviceIndex] != OK) || (operationCodes[deviceIndex] == requiredOperation) && (statuses[deviceIndex] == FINISHED))
                ++devicesReady;

            switch(statuses[deviceIndex]) {
                case IDLE:
                    current = idle;
                break;
                case PENDING:
                    current = pending;
                break;
                case FINISHED:
                    current = finished;
                break;
            }

            printf("device: %d, result: %d, operation: %d, status: %s\n", deviceIndex, results[deviceIndex], operationCodes[deviceIndex], current);
        }
    }
}
void printErrors()
{
    uint8_t deviceIndex = 0;
    int errorCode = 0;

    for (deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
        errorCode = checkLastError(deviceIndex, &errorCode);
        printf("Error code on device %d: %d\n", deviceIndex, errorCode);
    }
}


void printFlashBuffers()
{
    uint8_t index = 0;

    for (index = 0; index < devicesCount; ++index) {
        if (statuses[index] == FINISHED) {
            printFlashBuffer(index);
        }
    }
}
void printFlashBuffer(int deviceIndex)
{
    int index = 0;

    printf("Flash buffer for device %d (offset: %X, bytesToRead: %d): ", deviceIndex, READ_FLASH_OFFSET, READ_FLASH_BYTES);
    for (index = 0; index < READ_FLASH_BYTES; ++index) {
        printf("%d ", (uint16_t)(*(flashBuffers[deviceIndex] + index)));
    }
    printf("\n");
}
void printFrameBuffers()
{
    uint8_t index = 0;

    for (index = 0; index < devicesCount; ++index) {
        if (statuses[index] == FINISHED) {
            printFrameBuffer(index);
        }
    }
}
void printFrameBuffer(int deviceIndex)
{
    int index = 0;

    printf("Frame buffer for device %d: ", deviceIndex);
    for (index = 0; index < numOfPixelsInFrame[deviceIndex]; ++index) {
        printf("%d ", (uint16_t)(*(frameBuffers[deviceIndex] + index)));
    }
    printf("\n");
}

void exampleConnectDevices()
{
    int index = 0;
    int *result = &results[index];

    for (index = 0; index < devicesCount; ++index) {
        *result = connectToDevice(index);
        if (*result == OK) {
            printf("Connecting device %d .... success\n", index);           
        } else {
            printf("Connecting device %d .... failed, error code: %d\n", index, *result);
        }
    }
}

void exampleStartReadFlash()
{
    uint8_t deviceIndex = 0;

    for (deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
        if (results[deviceIndex] == OK) {
            free(flashBuffers[deviceIndex]);
            flashBuffers[deviceIndex] = (uint8_t*)calloc(READ_FLASH_BYTES, sizeof(uint8_t));

            results[deviceIndex] = readFlash(deviceIndex, flashBuffers[deviceIndex], READ_FLASH_OFFSET, READ_FLASH_BYTES);

            printf("readFlash command (operation %d) started on device %d with result %d\n", READ_FLASH, deviceIndex, results[deviceIndex]);
        }
    }

}

void exampleStartWriteFlash()
{
    uint8_t symbol = rand() % (UCHAR_MAX + 1);
    uint8_t deviceIndex = 0;

    printf("Filling flash memory with a random value: %d\n", symbol);

    for (deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
        if (results[deviceIndex] == OK) {
            free(flashBuffers[deviceIndex]);
            flashBuffers[deviceIndex] = (uint8_t*)calloc(WRITE_FLASH_BYTES, sizeof(uint8_t));

            memset(flashBuffers[deviceIndex], symbol, WRITE_FLASH_BYTES);

            results[deviceIndex] = writeFlash(deviceIndex, flashBuffers[deviceIndex], WRITE_FLASH_OFFSET, WRITE_FLASH_BYTES);

            printf("writeFlash command (operation %d) started on device %d with result %d\n", WRITE_FLASH, deviceIndex, results[deviceIndex]);
        }
    }
}

void exampleEraseFlash()
{
    uint8_t deviceIndex = 0;

    for (deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
        if (results[deviceIndex] == OK) {
            results[deviceIndex] = eraseFlash(deviceIndex);

            printf("eraseFlash command finished on device %d with result %d\n", deviceIndex, results[deviceIndex]);
        }
    }
}

void exampleSetAndStartAcquisition()
{
    int *result = NULL;
    uint8_t deviceIndex;

    const uint8_t scanMode = 3;
    const uint16_t numOfBlankScans = 0;
    uint16_t numOfScans = 1;

    for (deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
        result = &results[deviceIndex];

        if (*result == OK) {
            *result = clearMemory(deviceIndex);
        } else
            continue;

        if (*result == OK) {
            *result = setAcquisitionParameters(deviceIndex, numOfScans, numOfBlankScans, scanMode, EXPOSURE_MULTIPLIER * (deviceIndex + 1));
        } else {
            printf("(device %d): clearMemory() failed with code %d\n", deviceIndex, *result);
            continue;
        }

        if (*result == OK) {
            *result = getFrameFormat(deviceIndex, NULL, NULL, NULL, &numOfPixelsInFrame[deviceIndex]);
        } else {
            printf("(device %d): setAcquisitionParameters() failed with code %d\n", deviceIndex, *result);
            continue;
        }

        if (*result == OK) {
            printf("(device %d): numOfPixelsInFrame: %d\n", deviceIndex, numOfPixelsInFrame[deviceIndex]);
            printf("(device %d): started acquisition\n", deviceIndex);

            *result = triggerAcquisition(deviceIndex);
        } else {
            printf("(device %d): getFrameFormat() failed with code %d\n", deviceIndex, *result);
            continue;
        }

        if (*result != OK) {
            printf("readFramesExample (device %d): triggerAcquisition() failed with code %d\n", deviceIndex, *result);
            continue;
        }
    }
}

void exampleStartGetFrame()
{
    uint8_t deviceIndex = 0;

    uint8_t  statusFlags = 0;
    uint16_t framesInMemory = 0;
    uint8_t attempt = 1;
    clock_t localStart = 0;


    for (deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
        if (!numOfPixelsInFrame[deviceIndex]) {
            printf("Skipping getFrame for device %d: zero number of pixels in frame", deviceIndex);
            continue;
        }

        free(frameBuffers[deviceIndex]);
        frameBuffers[deviceIndex] = (uint16_t*)calloc(numOfPixelsInFrame[deviceIndex], sizeof(uint16_t));


        results[deviceIndex] = getStatus(deviceIndex, &statusFlags, &framesInMemory);
        if (results[deviceIndex] != OK) {
            printf("(device %d): getStatus() failed with code %d\n", deviceIndex, results[deviceIndex]);
            continue;
        }

        if (statusFlags & 2) {
            printf("(device %d): memory is full!\n", deviceIndex);
            results[deviceIndex] = clearMemory(deviceIndex);
            if (results[deviceIndex] != OK) {
                printf("(device %d): getStatus() failed with code %d\n", deviceIndex, results[deviceIndex]);
                continue;
            }

            results[deviceIndex] = triggerAcquisition(deviceIndex);
            if (results[deviceIndex] != OK) {
                printf("(device %d): triggerAcquisition() failed with code %d\n", deviceIndex, results[deviceIndex]);
                continue;
            }
        }

        do {
            if (!framesInMemory) {
                Sleep(1);
            }

            results[deviceIndex] = getStatus(deviceIndex, &statusFlags, &framesInMemory);
            if (results[deviceIndex] != OK) {
                printf("(device %d): getStatus() failed with code %d\n", deviceIndex, results[deviceIndex]);
                break;
            }


        } while (!framesInMemory || attempt++ > 3);

        if ((results[deviceIndex] == OK) && framesInMemory) {
            localStart = clock();
            results[deviceIndex] = getFrame(deviceIndex, frameBuffers[deviceIndex], 0xFFFF);            

            printf("getFrame command (operation %d) started on device %d with result %d\n", GET_FRAME, deviceIndex, results[deviceIndex]);
        }
    }
}


int main(void)
{
    int frameCounter = 0;   

    devicesCount = getDevicesCount();

    if (!devicesCount) {
        printf("Exiting: no devices found\n");
		printf("Press enter...\n");
		getchar();
        return 0;
    }

    initGlobals();

    exampleConnectDevices();

    exampleEraseFlash();

    exampleStartReadFlash();
    waitForOperationsFinish(READ_FLASH);
    printErrors();
    printFlashBuffers();

    exampleStartWriteFlash();
    waitForOperationsFinish(WRITE_FLASH);
    printErrors();

    exampleStartReadFlash();
    waitForOperationsFinish(READ_FLASH);
    printErrors();
    printFlashBuffers();    

    exampleEraseFlash();

    exampleStartReadFlash();
    waitForOperationsFinish(READ_FLASH);
    printErrors();
    printFlashBuffers();    

    exampleSetAndStartAcquisition();    

    while (frameCounter++ < FRAMES_REQUIRED) {
        exampleStartGetFrame();
        waitForOperationsFinish(GET_FRAME);
        printFrameBuffers();
    }

    clearGlobals();

    printf("Press enter...\n");
    getchar();
    return 0;
}

