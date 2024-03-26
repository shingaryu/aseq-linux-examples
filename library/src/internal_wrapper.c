#include <stdio.h>

#include "internal_wrapper.h"

int _verifyDeviceIndex(uint32_t deviceIndex)
{
    return (deviceIndex <= MAX_DEVICE_INDEX) ? 0 : 1;
}

void _initialSetup()
{
    int index = 0;
    for (index = MIN_DEVICE_INDEX; index <= MAX_DEVICE_INDEX; ++index) {
        _initNullContext(g_deviceThreads + index);
    }
}

void _setNullMutexes(ThreadContext_t* threadContext)
{
    threadContext->hNextTaskMutex = NULL;
    threadContext->hReadMutex = NULL;
    threadContext->hWriteMutex = NULL;
}

void _createMutexes(ThreadContext_t* threadContext)
{
    if (!threadContext->hWriteMutex) {
        threadContext->hWriteMutex = CreateMutex(NULL, FALSE, NULL);
    }

    if (!threadContext->hNextTaskMutex) {
        threadContext->hNextTaskMutex = CreateMutex(NULL, FALSE, NULL);
    }

    if (!threadContext->hReadMutex) {
        threadContext->hNextTaskMutex = CreateMutex(NULL, FALSE, NULL);
    }
}

void _setNullThreadTask(ThreadTask_t* threadTask)
{
    if (threadTask == NULL) {
        return;
    }

    threadTask->args = NULL;
    threadTask->sizeOfArgs = 0;
    threadTask->currentOperation = NO_OPERATION;
    threadTask->operationStatus = IDLE;
}
void _setNullEvents(ThreadEvents* threadevents)
{
    if (threadevents == NULL) {
        return;
    }

    threadevents->hNextTaskAvailable = NULL;
    threadevents->hThreadContextSaved = NULL;
    threadevents->hTaskInProgress = NULL;
}

void _initNullContext(ThreadContext_t* threadContext)
{
    if (threadContext == NULL) {
        return;
    }

    threadContext->worker = NULL;
    threadContext->index = 0;

    _setNullMutexes(threadContext);

    threadContext->pDeviceState = NULL;

    _setNullEvents(&(threadContext->events));

    threadContext->flags.stopOperations = 0;

    _setNullThreadTask(&(threadContext->currentTask));
    _setNullThreadTask(&(threadContext->nextTask));

    threadContext->lastNumOfPixelsInFrame = 0;
    threadContext->numOfPixelsInFrame = 0;

    threadContext->error = 0;
}


void _createEvents(ThreadContext_t* threadContext)
{
    if (threadContext == NULL) {
        return;
    }

    if (threadContext->events.hNextTaskAvailable == NULL) {
        threadContext->events.hNextTaskAvailable = CreateEvent(NULL, TRUE, FALSE, NULL);
    }

    if (threadContext->events.hThreadContextSaved == NULL) {
        threadContext->events.hThreadContextSaved = CreateEvent(NULL, TRUE, FALSE, NULL);
    }

    if (threadContext->events.hTaskInProgress == NULL) {
        threadContext->events.hTaskInProgress = CreateEvent(NULL, TRUE, FALSE, NULL);
    }



}

void _resetEvents(ThreadContext_t* threadContext)
{
    if (threadContext == NULL) {
        return;
    }

    if (threadContext->events.hNextTaskAvailable != NULL) {
        ResetEvent(threadContext->events.hNextTaskAvailable);
    }

    if (threadContext->events.hThreadContextSaved != NULL) {
        ResetEvent(threadContext->events.hThreadContextSaved);
    }

    if (threadContext->events.hTaskInProgress != NULL) {
        ResetEvent(threadContext->events.hTaskInProgress);
    }
}

void _freeAndNullThreadTask(ThreadTask_t* threadTask)
{
    if (threadTask != NULL) {
        return;
    }

    if (threadTask->args != NULL) {
        free(threadTask->args);
    }

    _setNullThreadTask(threadTask);
}


void _freeAndNullThreadContext(ThreadContext_t* threadContext)
{
    int result = 0;

    if (threadContext == NULL) {
        return;
    }

    if (threadContext->worker != NULL) {
        WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
        threadContext->flags.stopOperations = 1;
        SetEvent(threadContext->events.hNextTaskAvailable);
        ReleaseMutex(threadContext->hWriteMutex);

        WaitForSingleObject(threadContext->worker, INFINITE);
        result = GetLastError();
        //printf("setDefaultContext for thread number %d: thread finished with error: %d", threadContext->index, result);
    }

    if (threadContext->hWriteMutex != NULL) {
        ReleaseMutex(threadContext->hWriteMutex);
    }

    if (threadContext->pDeviceState) {
        //TODO: should we disconnect device?
        free(threadContext->pDeviceState);
    }

    _resetEvents(threadContext);

    threadContext->worker = NULL;
    threadContext->index = 0;
    threadContext->pDeviceState = NULL;

    _freeAndNullThreadTask(&(threadContext->currentTask));
    _freeAndNullThreadTask(&(threadContext->nextTask));

    threadContext->flags.stopOperations = 0;

    threadContext->numOfPixelsInFrame = 0;
    threadContext->error = 0;
}

void _setDefaultContext(ThreadContext_t* threadContext)
{
    if (threadContext == NULL) {
        return;
    }

    _freeAndNullThreadContext(threadContext);
    _createMutexes(threadContext);
    _createEvents(threadContext);
}

int _getFrameWrapper(ThreadContext_t* threadContext)
{
    int result = -1;

    uint16_t numOfFrame = 0;
    uintptr_t pointer = 0;
    uint16_t *framePixelsBuffer = NULL;

    uint8_t threadSafeMode = 1;

    char* iterator = NULL;

    //uint16_t *framePixelsBuffer, uint16_t numOfFrame

    if (threadContext == NULL) {
        return INPUT_PARAMETER_NOT_INITIALIZED;
    }

    iterator = threadContext->currentTask.args;
    pointer = *(uintptr_t*)iterator;
    framePixelsBuffer = (uint16_t*)pointer;
    iterator += sizeof(uintptr_t);
    numOfFrame = *(uint16_t*)iterator;

    result = getFrameFromSingleDevice(framePixelsBuffer, numOfFrame, threadSafeMode, &(threadContext->pDeviceState));
    return result;
}

int _readFlashWrapper(ThreadContext_t *threadContext)
{
    int result = -1;

    uint8_t *buffer = NULL;
    uintptr_t pointer = 0;
    uint32_t absoluteOffset = 0;
    uint32_t bytesToRead = 0;

    uint8_t threadSafeMode = 1;

    char* iterator = NULL;

    if (threadContext == NULL) {
        return INPUT_PARAMETER_NOT_INITIALIZED;
    }

    iterator = threadContext->currentTask.args;
    pointer = *(uintptr_t*)iterator;
    buffer = (uint8_t*)pointer;
    iterator += sizeof(uintptr_t);
    absoluteOffset = *(uint32_t*)iterator;
    iterator += sizeof(uint32_t);
    bytesToRead = *(uint32_t*)iterator;

    result = readFlashFromSingleDevice(buffer, absoluteOffset, bytesToRead, threadSafeMode, &(threadContext->pDeviceState));
    return result;
}

int _writeFlashWrapper(ThreadContext_t *threadContext)
{
    int result = -1;

    uint8_t *buffer = NULL;
    uintptr_t pointer = 0;
    uint32_t absoluteOffset = 0;
    uint32_t bytesToWrite = 0;

    uint8_t threadSafeMode = 1;

    char* iterator = NULL;

    if (threadContext == NULL) {
        return INPUT_PARAMETER_NOT_INITIALIZED;
    }

    iterator = threadContext->currentTask.args;
    pointer = *(uintptr_t*)iterator;
    buffer = (uint8_t*)pointer;
    iterator += sizeof(uintptr_t);
    absoluteOffset = *(uint32_t*)iterator;
    iterator += sizeof(uint32_t);
    bytesToWrite = *(uint32_t*)iterator;

    result = writeFlashOnSingleDevice(buffer, absoluteOffset, bytesToWrite, threadSafeMode, &(threadContext->pDeviceState));
    return result;
}


unsigned __stdcall _threadDeviceOperations(void *data)
{
    int index = 0;
    int errorCode = OK;
    ThreadContext_t *threadContext = NULL;
    index = *(int*)data;
    threadContext = g_deviceThreads + index;
    SetEvent(threadContext->events.hThreadContextSaved);

    while(1) {        
        WaitForSingleObject(threadContext->events.hNextTaskAvailable, INFINITE);

        WaitForSingleObject(threadContext->hNextTaskMutex, INFINITE);
            ResetEvent(threadContext->events.hNextTaskAvailable);
        ReleaseMutex(threadContext->hNextTaskMutex);

        WaitForSingleObject(threadContext->hWriteMutex, INFINITE);
            WaitForSingleObject(threadContext->hReadMutex, INFINITE);
                threadContext->currentTask.operationStatus = IDLE;
                threadContext->error = 0;
            ReleaseMutex(threadContext->hReadMutex);

            if (threadContext->flags.stopOperations) {
                ReleaseMutex(threadContext->hWriteMutex);
                break;
            }

            WaitForSingleObject(threadContext->hReadMutex, INFINITE);
                WaitForSingleObject(threadContext->hNextTaskMutex, INFINITE);
                    _freeAndNullThreadTask(&(threadContext->currentTask));
                    memcpy(&(threadContext->currentTask), &(threadContext->nextTask), sizeof(ThreadTask_t));
                ReleaseMutex(threadContext->hNextTaskMutex);

                threadContext->currentTask.operationStatus = PENDING;

                SetEvent(threadContext->events.hTaskInProgress);
            ReleaseMutex(threadContext->hReadMutex);

            switch (threadContext->currentTask.currentOperation) {
            case GET_FRAME: {
                errorCode = _getFrameWrapper(threadContext);
                break;
            }
            case READ_FLASH: {
                errorCode = _readFlashWrapper(threadContext);
                break;
            }
            case WRITE_FLASH: {
                 errorCode = _writeFlashWrapper(threadContext);
                 break;
            }
            default:
                printf("_threadDeviceOperations for thread %d: unsupported current operation: %d", threadContext->index, threadContext->currentTask.currentOperation);
                getchar();
                break;
            }

            WaitForSingleObject(threadContext->hReadMutex, INFINITE);
                threadContext->currentTask.operationStatus = FINISHED;
                threadContext->error = errorCode;
            ReleaseMutex(threadContext->hReadMutex);

        ReleaseMutex(threadContext->hWriteMutex);
    }

    return 0;
}
