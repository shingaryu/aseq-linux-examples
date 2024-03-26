#ifndef INTERNAL_WRAPPER_H
#define INTERNAL_WRAPPER_H

#include <windows.h>
#include <process.h>
#include "libspectr.h"


#define MIN_DEVICE_INDEX 0
#define MAX_DEVICE_INDEX 9


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef struct ThreadFlags {
    uint8_t stopOperations : 1;

} ThreadFlags;

typedef struct ThreadEvents {
   HANDLE hThreadContextSaved;
   HANDLE hNextTaskAvailable;
   HANDLE hTaskInProgress;
} ThreadEvents;

#ifndef WRAPPER_ENUMS
#define WRAPPER_ENUMS
typedef enum LibOperation_t {NO_OPERATION, SET_FRAME_FORMAT, SET_EXPOSURE, SET_ACQUISITION_PARAMETERS, SET_ALL_PARAMETERS, SET_EXTERNAL_TRIGGER, SET_OPTICAL_TRIGGER, TRIGGER_ACQUISITION, GET_STATUS, GET_ACQUISITION_PARAMETERS,
                              GET_FRAME_FORMAT, GET_FRAME, CLEAR_MEMORY, ERASE_FLASH, READ_FLASH, WRITE_FLASH, RESET_DEVICE, DETACh_DEVICE} LibOperation_t;

typedef enum OperationStatus_t {IDLE, PENDING, FINISHED} OperationStatus_t;

#endif




typedef struct ThreadTask_t {
    LibOperation_t currentOperation;
    OperationStatus_t operationStatus;
    char *args;
    uint32_t sizeOfArgs;
} ThreadTask_t;

/*

//init mutexes
//init events
//init flags

Read mutex is used for all the parameters which can be read from the user's application:

currentTask context:
    currentOperation
    currentOperationStatus
    errorCode
*/


typedef struct ThreadContext_t {
    uint16_t index;
    HANDLE worker;

    HANDLE hWriteMutex;
    HANDLE hReadMutex;
    HANDLE hNextTaskMutex;

    char *pDeviceState;

    ThreadEvents events;
    ThreadFlags flags;

    ThreadTask_t currentTask;
    ThreadTask_t nextTask;

    uint16_t lastNumOfPixelsInFrame;
    uint16_t numOfPixelsInFrame;

    int error;
} ThreadContext_t;

ThreadContext_t g_deviceThreads[10];

unsigned __stdcall _threadDeviceOperations(void *data);

int _verifyDeviceIndex(uint32_t deviceIndex);
void _initialSetup();
void _initNullContext(ThreadContext_t* threadContext);

void _freeAndNullThreadTask(ThreadTask_t* threadTask);
void _freeAndNullThreadContext(ThreadContext_t* threadContext);
void _setDefaultContext(ThreadContext_t* threadContext);

int _getFrameWrapper(ThreadContext_t *threadContext);
int _readFlashWrapper(ThreadContext_t *threadContext);
int _writeFlashWrapper(ThreadContext_t *threadContext);

#ifndef WRAPPER_ERROR_CODES
#define WRAPPER_ERROR_CODES
    #define ERROR_WRONG_DEVICE_INDEX 1
#endif

#endif
