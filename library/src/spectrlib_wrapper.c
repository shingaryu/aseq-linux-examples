#include "spectrlib_wrapper.h"
#include "internal_wrapper.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifndef LIBSHAREDWRAPPER_ERROR_CODES
#define LIBSHAREDWRAPPER_ERROR_CODES
    #define ERROR_WRONG_DEVICE_INDEX 1
#endif

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason) {
        case DLL_PROCESS_ATTACH:
            _initialSetup();
            break;
        case DLL_PROCESS_DETACH:
            disconnectLibrary();
            break;
    }
    return TRUE;
}
#endif

void disconnectLibrary()
{
    int result = -1, deviceIndex = 0;
    ThreadContext_t* threadContext = NULL;

    for (deviceIndex = 0; deviceIndex <= MAX_DEVICE_INDEX; ++deviceIndex) {
        threadContext = g_deviceThreads + deviceIndex;
        _freeAndNullThreadContext(threadContext);
    }
}

int getCurrentOperationInfo(uint32_t deviceIndex, int* operationCode, int* operationStatus)
{
    int error = OK;
    ThreadContext_t* threadContext = NULL;
    int tempCode = NO_OPERATION;
    int tempStatus = IDLE;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    if (threadContext->worker == NULL) {
        return THREAD_NOT_INITIALIZED;
    }    

    WaitForSingleObject(threadContext->hReadMutex, INFINITE);
    tempCode = threadContext->currentTask.currentOperation;
    tempStatus = threadContext->currentTask.operationStatus;
    ReleaseMutex(threadContext->hReadMutex);

    if (operationCode) {
        *operationCode = tempCode;
    }

    if (operationStatus) {
        *operationStatus = tempStatus;
    }

    return OK;
}

int checkLastError(uint32_t deviceIndex, int* deviceError)
{
    int error = OK;
    ThreadContext_t* threadContext = NULL;
    int tempError = OK;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    if (threadContext->worker == NULL) {
        return THREAD_NOT_INITIALIZED;
    }

    WaitForSingleObject(threadContext->hReadMutex, INFINITE);
    tempError = threadContext->error;
    ReleaseMutex(threadContext->hReadMutex);

    if (deviceError) {
        *deviceError = tempError;
    }

    return OK;
}

int connectToDevice(uint32_t deviceIndex)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    //FIXME: check if this is correct initialization
    _setDefaultContext(threadContext);

    //NOTE: after _setDefaultContext the worker is stopped, and we can work without mutex
    error = connectToSingleDeviceByIndex(deviceIndex, 1, &(threadContext->pDeviceState));

    if (error != OK) {
        return error;
    }

    threadContext->index = deviceIndex;
    threadContext->worker = (HANDLE)_beginthreadex(NULL, 0, _threadDeviceOperations, &threadContext->index, 0, NULL);    //CREATE_SUSPENDED
    WaitForSingleObject(threadContext->events.hThreadContextSaved, INFINITE);
    ResetEvent(threadContext->events.hThreadContextSaved);

    return OK;
}

int detachDevice(uint32_t deviceIndex)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = detachSingleDevice(threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);

    return error;
}

int resetDevice(uint32_t deviceIndex)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = resetSingleDevice(threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);

    return error;
}

uint32_t getDevicesCount()
{
    uint32_t result = getAllDevicesCount();
    return result;
}

int setFrameFormat(uint32_t deviceIndex, uint16_t numOfStartElement, uint16_t numOfEndElement, uint8_t reductionMode, uint16_t *numOfPixelsInFrame)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = setFrameFormatOnSingleDevice(numOfStartElement, numOfEndElement, reductionMode, numOfPixelsInFrame, threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);
    return error;
}

int setExposure(uint32_t deviceIndex, uint32_t timeOfExposure, uint8_t force)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = setExposureOnSingleDevice(timeOfExposure, force, threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);
    return error;
}

int setAcquisitionParameters(uint32_t deviceIndex, uint16_t numOfScans, uint16_t numOfBlankScans, uint8_t scanMode, uint32_t timeOfExposure)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = setAcquisitionParametersOnSingleDevice(numOfScans, numOfBlankScans, scanMode, timeOfExposure, threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);
    return error;
}

int setMultipleParameters(uint32_t deviceIndex, uint16_t numOfScans, uint16_t numOfBlankScans, uint8_t scanMode, uint32_t timeOfExposure, uint8_t enableMode, uint8_t signalFrontMode)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = setMultipleParametersOnSingleDevice(numOfScans, numOfBlankScans, scanMode, timeOfExposure, enableMode, signalFrontMode, threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);
    return error;
}

int setExternalTrigger(uint32_t deviceIndex, uint8_t enableMode, uint8_t signalFrontMode)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = setExternalTriggerOnSingleDevice(enableMode, signalFrontMode, threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);
    return error;
}

int setOpticalTrigger(uint32_t deviceIndex, uint8_t enableMode, uint16_t pixel, uint16_t threshold)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = setOpticalTriggerOnSingleDevice(enableMode, pixel, threshold, threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);

    return error;
}

int triggerAcquisition(uint32_t deviceIndex)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = triggerAcquisitionOnSingleDevice(threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);

    return error;
}

int getStatus(uint32_t deviceIndex, uint8_t *statusFlags, uint16_t *framesInMemory)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = getStatusFromSingleDevice(statusFlags, framesInMemory, threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);

    return error;
}

int getAcquisitionParameters(uint32_t deviceIndex, uint16_t* numOfScans, uint16_t* numOfBlankScans, uint8_t *scanMode, uint32_t* timeOfExposure)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = getAcquisitionParametersFromSingleDevice(numOfScans, numOfBlankScans, scanMode, timeOfExposure, threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);

    return error;
}

int getFrameFormat(uint32_t deviceIndex, uint16_t *numOfStartElement, uint16_t *numOfEndElement, uint8_t *reductionMode, uint16_t *numOfPixelsInFrame)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = getFrameFormatFromSingleDevice(numOfStartElement, numOfEndElement, reductionMode, numOfPixelsInFrame, threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);

    return error;
}

int getFrame(uint32_t deviceIndex, uint16_t *pixelsBuffer, uint16_t numOfFrame)
{
    ThreadContext_t* threadContext = NULL;
    int error = 0;
    uintptr_t pointer = 0;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    if (threadContext->worker == NULL) {
        error = connectToDevice(deviceIndex);

        if (error != OK) {
            return error;
        }
    }

    WaitForSingleObject(threadContext->hNextTaskMutex, INFINITE);
        _freeAndNullThreadTask(&threadContext->nextTask);

        threadContext->nextTask.currentOperation = GET_FRAME;

        pointer = (uintptr_t)pixelsBuffer;

        threadContext->nextTask.sizeOfArgs = sizeof(uintptr_t) + sizeof(uint16_t);
        threadContext->nextTask.args = (char*)malloc(threadContext->nextTask.sizeOfArgs);
        memcpy(threadContext->nextTask.args, &pointer, sizeof(uintptr_t));
        memcpy(threadContext->nextTask.args + sizeof(uintptr_t), &numOfFrame, sizeof(uint16_t));

          SetEvent(threadContext->events.hNextTaskAvailable);
    ReleaseMutex(threadContext->hNextTaskMutex);

    WaitForSingleObject(threadContext->events.hTaskInProgress, INFINITE);
    ResetEvent(threadContext->events.hTaskInProgress);

    return 0;
}

int clearMemory(uint32_t deviceIndex)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
    error = clearMemoryOnSingleDevice(threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);

    return error;
}

int eraseFlash(uint32_t deviceIndex)
{
    ThreadContext_t* threadContext = NULL;
    int error = OK;
    uint8_t threadSafeMode = 1;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
        error = eraseFlashOnSingleDevice(threadSafeMode, &(threadContext->pDeviceState));
    ReleaseMutex(threadContext->hWriteMutex);

    return error;
}

int readFlash(uint32_t deviceIndex, uint8_t *buffer, uint32_t absoluteOffset, uint32_t bytesToRead)
{
    ThreadContext_t* threadContext = NULL;
    int error = 0;
    uintptr_t pointer = 0;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    if (threadContext->worker == NULL) {
        error = connectToDevice(deviceIndex);

        if (error != OK) {
            return error;
        }
    }

    WaitForSingleObject(threadContext->hNextTaskMutex, INFINITE);

        _freeAndNullThreadTask(&threadContext->nextTask);

        threadContext->nextTask.currentOperation = READ_FLASH;

        pointer = (uintptr_t)buffer;

        threadContext->nextTask.sizeOfArgs = sizeof(uintptr_t) + sizeof(uint32_t) + sizeof(uint32_t);
        threadContext->nextTask.args = (char*)malloc(threadContext->nextTask.sizeOfArgs);
        memcpy(threadContext->nextTask.args, &pointer, sizeof(uintptr_t));
        memcpy(threadContext->nextTask.args + sizeof(uintptr_t), &absoluteOffset, sizeof(uint32_t));
        memcpy(threadContext->nextTask.args + sizeof(uintptr_t) + sizeof(uint32_t), &bytesToRead, sizeof(uint32_t));

        SetEvent(threadContext->events.hNextTaskAvailable);
    ReleaseMutex(threadContext->hNextTaskMutex);

    WaitForSingleObject(threadContext->events.hTaskInProgress, INFINITE);
    ResetEvent(threadContext->events.hTaskInProgress);

    return OK;
}

int writeFlash(uint32_t deviceIndex, uint8_t *buffer, uint32_t absoluteOffset, uint32_t bytesToWrite)
{
    ThreadContext_t* threadContext = NULL;
    int error = 0;
    uintptr_t pointer = 0;

    error = _verifyDeviceIndex(deviceIndex);
    if (error != OK) {
        return error;
    }

    threadContext = g_deviceThreads + deviceIndex;

    if (threadContext->worker == NULL) {
        error = connectToDevice(deviceIndex);

        if (error != OK) {
            return error;
        }
    }

    WaitForSingleObject(threadContext->hNextTaskMutex, INFINITE);
        _freeAndNullThreadTask(&threadContext->nextTask);

        threadContext->nextTask.currentOperation = WRITE_FLASH;

        pointer = (uintptr_t)buffer;

        threadContext->nextTask.sizeOfArgs = sizeof(uintptr_t) + sizeof(uint32_t) + sizeof(uint32_t);
        threadContext->nextTask.args = (char*)malloc(threadContext->nextTask.sizeOfArgs);
        memcpy(threadContext->nextTask.args, &pointer, sizeof(uintptr_t));
        memcpy(threadContext->nextTask.args + sizeof(uintptr_t), &absoluteOffset, sizeof(uint32_t));
        memcpy(threadContext->nextTask.args + sizeof(uintptr_t) + sizeof(uint32_t), &bytesToWrite, sizeof(uint32_t));

        SetEvent(threadContext->events.hNextTaskAvailable);
    ReleaseMutex(threadContext->hNextTaskMutex);

    WaitForSingleObject(threadContext->events.hTaskInProgress, INFINITE);
    ResetEvent(threadContext->events.hTaskInProgress);

    return OK;
}
