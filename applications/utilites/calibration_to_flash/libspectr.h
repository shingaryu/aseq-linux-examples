/** \file
 * \defgroup API libspectr API
 */

#ifndef LIBSHARED_AND_STATIC_EXPORT_H
#define LIBSHARED_AND_STATIC_EXPORT_H

#if defined (_WIN32)
    #ifndef LIBSHARED_AND_STATIC_EXPORT
        #if !defined(AS_CORE_LIBRARY)
            #ifdef spectrlib_shared_EXPORTS
                #define LIBSHARED_AND_STATIC_EXPORT __declspec(dllexport)
            #else
                #define LIBSHARED_AND_STATIC_EXPORT __declspec(dllimport)
            #endif
        #else
            #define LIBSHARED_AND_STATIC_EXPORT
        #endif       
    #endif
  
    #ifndef LIBSHARED_AND_STATIC_DEPRECATED
        #define LIBSHARED_AND_STATIC_DEPRECATED __declspec(deprecated)
        #define LIBSHARED_AND_STATIC_DEPRECATED_EXPORT LIBSHARED_AND_STATIC_EXPORT __declspec(deprecated)
        #define LIBSHARED_AND_STATIC_DEPRECATED_NO_EXPORT LIBSHARED_AND_STATIC_NO_EXPORT __declspec(deprecated)
    #endif
#else /* defined (_WIN32) */
    #define LIBSHARED_AND_STATIC_EXPORT
#endif

#ifndef AS_CORE_LIBRARY
    #ifdef __cplusplus
        extern "C" {
    #endif
#endif

#include <stdint.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#ifndef DEVICE_INFO
#define DEVICE_INFO
typedef struct DeviceInfo_t{
      char* serialNumber;
      struct DeviceInfo_t *next;
} DeviceInfo_t;
#endif


/** \brief Frees memory allocated by deviceContextPtr

    \param[in] deviceContextPtr
    \parblock
    This pointer should not be NULL
    The handle inside will be initialized with the device state information required for all the other functions.
    If the requested device is already connected (dereferenced pointer value does not equal 0), disconnects and initializes the state again
    (in thread-safe mode)
    \endparblock

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int disconnectDeviceContext(uintptr_t* deviceContextPtr);

/** \brief Finds the requested device by the provided serial number and connects to it.
    This function or its analog connectToSingleDeviceByIndex should always precede all other library operations.

    \param[in] serialNumber
    \parblock
    If the serialNumber pointer is NULL, the function will connect to the first found device with VID = 0xE220 and PID = 0x0100
    \endparblock

    \param[out] deviceContextPtr
    \parblock
    This pointer should not be NULL
    The handle inside will be initialized with the device state information required for all the other functions.
    If the requested device is already connected (dereferenced pointer value does not equal 0), disconnects and initializes the state again
    (in thread-safe mode)
    \endparblock

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int connectToDeviceBySerial(const char * const serialNumber, uintptr_t* deviceContextPtr);

/** \brief Connects to the required device by index
    This function or its analog connectToSingleDeviceBySerial should always precede all other library operations.

    \param[in] index
    \parblock
    The value of the index parameter can vary from 0 to n-1 (where n is the number of the connected devices)
    If only one device is connected use 0 as the index parameter.
    \endparblock    

    \param[out] deviceContextPtr
    \parblock
    If the threadSafeMode is set to true, this pointer will be initialized with the device state information required for all the other functions.
    (Provide this initialized pointer to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again
    (in thread-safe mode)
    \endparblock

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int connectToDeviceByIndex(unsigned int index, uintptr_t *deviceContextPtr);

/* Deprecated - left for internal use only
LIBSHARED_AND_STATIC_EXPORT void disconnectDevice();
*/

/** \brief Returns the number of the connected devices */
LIBSHARED_AND_STATIC_EXPORT uint32_t getDevicesCount();

/** \brief Obtains the list of all connected devices.
Run clearDevicesInfo() with the result of this function to clear it [Mandatory]
structure information:
struct DeviceInfo_t {
      char* serial; //descriptor to the string with serial number
      struct DeviceInfo_t *next; //descriptor to the next element of the list or null if last element
}
    \returns the list of all connected devices (afterwards should be freed with clearDevicesInfo function!)
*/
LIBSHARED_AND_STATIC_EXPORT DeviceInfo_t * getDevicesInfo();

/** \brief Clears the list of all connected devices obtained by getDevicesInfo() function */
LIBSHARED_AND_STATIC_EXPORT void clearDevicesInfo(DeviceInfo_t *devices);

/** \brief Sets frame parameters
Note: this function clears the memory and stops the current acquisition

\param[in] numOfStartElement
\parblock
Minimum value is 0
Maximum value is 3647

if numOfStartElement == numOfEndElement, a frame will contain (32 starting elements) + (1 user element) + (14 final elements).
\endparblock

\param[in] numOfEndElement
\parblock
Minimum value is 0
Maximum value is 3647

if numOfStartElement == numOfEndElement, a frame will contain (32 starting elements) + (1 user element) + (14 final elements).
\endparblock

\param[in] reductionMode
\parblock
reductionMode defines the mode of pixels averaging
    0 - no average
    1 - average of 2
    2 - average of 4
    3 - average of 8
\endparblock

\param[out] numOfPixelsInFrame
The numOfPixelsInFrame parameter is used to return the frame size in pixels. Frame size in bytes = (2 * numOfPixelsInFrame)
Use NULL to exclude this parameter.
\parblock
\endparblock

\param[out] deviceContextPtr
\parblock
If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
(Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
\endparblock

\details {
    sends:
    outReport[1]=4;
    outReport[2]=LO(startElement);
    outReport[3]=HI(startElement);
    outReport[4]=LO(endElement);
    outReport[5]=HI(endElement);
    outReport[6]=reduce;

    gets:
    inReport[0]=0x84;
    inReport[1]=errorCode;
    inReport[2]=LO(frameElements);
    inReport[3]=HI(frameElements);
}

   \ingroup API

   \returns   This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int setFrameFormat(uint16_t numOfStartElement, uint16_t numOfEndElement, uint8_t reductionMode, uint16_t *numOfPixelsInFrame, uintptr_t *deviceContextPtr);

/** \brief Sets exposure parameter

Can be called in the middle of acquisition, exposure will be applied on the next frame

    \param[in] timeOfExposure = multiple of 10 us (microseconds)
    \param[in] force - set force
    \param[in] deviceContextPtr
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \returns
       This function returns 0 on success and error code in case of error.

    \details

    sends:
    outReport[1] = 2;
    outReport[2] = exposure[0];
    outReport[3] = exposure[1];
    outReport[4] = exposure[2];
    outReport[5] = exposure[3];
    outReport[6] = force;

    gets:
    inReport[0] = 0x82;
    inReport[1] = errorCode;

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int setExposure(uint32_t timeOfExposure, uint8_t force, uintptr_t *deviceContextPtr);

/** \brief Sets acquisition parameters
    \note current acquisition will be stopped by calling this function

    \param[in] numOfScans - up to 137 full spectra
    \param[in] numOfBlankScans - check the description of the scanMode parameter below
    \param[in] scanMode
    \parblock
    0 - continuous scanMode

    CCD is read continuously. On trigger numOfScans frames will be stored in memory, with numOfBlankScans blank frames between them.

    1 - first frame idle scan mode

    no activity on CCD before trigger. On trigger numOfScans frames with numOfBlankScans blank frames between them will be read automaticly, then CCD enters idle mode until the next trigger.

    2 - every frame idle scan mode

    no activity on CCD before trigger. Every frame, including blank frames will be read only on trigger.

    3 - frame averaging mode. CCD is read continuously. Every numOfScans frames are averaged, result can be loaded with getFrame(0xFFFF) function. numOfBlankScans in this mode does not make sense and has to be set to 0
    \endparblock
    \param[in] timeOfExposure multiple of 10 us (microseconds)

    \param[in] deviceContextPtr
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \details
    sends:
    outReport[1] = 3;
    outReport[2] = LO(scans);
    outReport[3] = HI(scans);
    outReport[4] = LO(blankScans);
    outReport[5] = HI(blankScans);
    outReport[6] = scanMode;

    gets:
    inReport[0]=0x83;
    inReport[1]=errorCode;

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int setAcquisitionParameters(uint16_t numOfScans, uint16_t numOfBlankScans, uint8_t scanMode, uint32_t timeOfExposure, uintptr_t *deviceContextPtr);

/** \brief Sets multiple parameters - combination of setAcquisitionParametersOnSingleDevice and setExternalTriggerOnSingleDevice functions
    Clears memory and sets the required parameters (the parameters for the setAcquisitionParameters and setExternalTrigger functions). If enableMode = 0 or signalFrontMode = 0 (external trigger disabled) this function also triggers the acquisition
    \param[in] numOfScans
    \param[in] numOfBlankScans
    \param[in] scanMode
    \param[in] timeOfExposure
    \param[in] enableMode
    \param[in] signalFrontMode    
    \param[in] deviceContextPtr
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int setMultipleParameters(uint16_t numOfScans, uint16_t numOfBlankScans, uint8_t scanMode, uint32_t timeOfExposure, uint8_t enableMode, uint8_t signalFrontMode, uintptr_t *deviceContextPtr);

/** \brief Sets the external acquisition trigger
    \param[in] enableMode
    \parblock
    enableMode:
    0 - trigger disabled
    1 - trigger enabled
    2 - one time trigger
    \endparblock
    \param[in] signalFrontMode
    \parblock
    triggerFront:
    0 - trigger disabled
    1 - front rising
    2 - front falling
    3 - both rising and fall
    \endparblock
    \param[in] deviceContextPtr
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int setExternalTrigger(uint8_t enableMode, uint8_t signalFrontMode, uintptr_t *deviceContextPtr);

/** \brief Sets the optical atrigger
    \note Only applicable in scanMode = 0 (check the acquisition parameters)

    \param[in] enableMode
    \parblock
    enableMode = 0 trigger disabled
    enableMode = 1 trigger for falling edge
    enableMode = 2 trigger on threshold
    enableMode = 0x81 one time trigget for rising edge
    enableMode = 0x82 one time trigger for falling edge
    \endparblock
    \param[in] pixel
    \parblock
    Pixel number (for 0 to 3639 range) for with exiding level should be detected (for optical trigger only). Does not depend on frame format.
    \endparblock
    \param[in] threshold

    \param[out] deviceContextPtr
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \details
    outReport[1] = 0x0B;
    outReport[2] = triggerMode;
    outReport[3] = LO(pixel);
    outReport[4] = HI(pixel);
    outReport[5] = LO(threshold);
    outReport[6] = HI(threshold);

    inReport[0] = 0x8B;
    inReport[1] = errorCode;

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int setOpticalTrigger(uint8_t enableMode, uint16_t pixel, uint16_t threshold, uintptr_t *deviceContextPtr);

/** \brief Start acquisition by software    
    \param[in] deviceContextPtr
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \details
    outReport[1]=6;

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.

*/
LIBSHARED_AND_STATIC_EXPORT int triggerAcquisition(uintptr_t *deviceContextPtr);

/** \brief Gets the device status.
    \param[out] statusFlags - provide an initialized pointer or NULL to skip this parameter fetch
    \param[out] framesInMemory - provide an initialized pointer or NULL to skip this parameter fetch

    \param[in] deviceContextPtr
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock


    \details
    sends:
    outReport[0] = 0
    outReport[1] = 1;
    outReport[2] = 0;

    gets:
    inReport[0] = 0x81;
    inReport[1] = flags;
    inReport[2] = LO(framesInMemory);
    inReport[3] = HI(framesInMemory);

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int getStatus(uint8_t *statusFlags, uint16_t *framesInMemory,  uintptr_t *deviceContextPtr);

/** \brief Returns the same values as set by setAcquisitionParameters
    \param[out] numOfScans - provide an initialized pointer or NULL to skip this parameter fetch
    \param[out] numOfBlankScans - provide an initialized pointer or NULL to skip this parameter fetch
    \param[out] scanMode - provide an initialized pointer or NULL to skip this parameter fetch
    \param[out] timeOfExposure - provide an initialized pointer or NULL to skip this parameter fetch

    \param[in] deviceContextPtr
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int getAcquisitionParameters(uint16_t* numOfScans, uint16_t* numOfBlankScans, uint8_t *scanMode, uint32_t* timeOfExposure, uintptr_t *deviceContextPtr);

/** \brief Returns the same values as set by setFrameFormat
    \param[out] numOfStartElement
    \param[out] numOfEndElement
    \param[out] reductionMode
    \param[out] numOfPixelsInFrame

    \param[in] deviceContextPtr
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \details
    outReport[1] = 8;

    inReport[0] = 0x88;
    inReport[1] = LO(startElement);
    inReport[2] = HI(startElement);
    inReport[3] = LO(endElement);
    inReport[4] = HI(endElement);
    inReport[5] = redux;
    inReport[6] = LO(numOfFrameElements);
    inReport[7] = HI(numOfFrameElements);

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int getFrameFormat(uint16_t *numOfStartElement, uint16_t *numOfEndElement, uint8_t *reductionMode, uint16_t *numOfPixelsInFrame, uintptr_t *deviceContextPtr);

/** \brief Gets frame
    \param[out] framePixelsBuffer - provide an initialized pointer to the buffer of unsigned short elements.
    \param[in] numOfFrame
    \parblock
    numOfFrame - first frame in memory is number 0, second is 1, etc
    numOfFrame = 0xFFFF - calculate averaged spectra (for averaging mode), for all other modes function will return last captured frame
    \endparblock

    \param[in] deviceStateStruct
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \details
    outReport[1]=0x0A;
    outReport[2]=LO(offset);
    outReport[3]=HI(offset);
    outReport[4]=LO(frameNum);
    outReport[5]=HI(frameNum);
    outReport[6]=packets;

    inReport[0]=0x8A;
    inReport[1]=LO(offset);
    inReport[2]=HI(offset);
    inReport[3]=packetsRemaining_or_IsError;
    inReport[4]=LO(frame[offset]);
    inReport[5]=HI(frame[offset]);
    inReport[6]=LO(frame[offset+1]);
    inReport[7]=HI(frame[offset+1]);
    ...
    inReport[62]=LO(frame[offset+29]);
    inReport[63]=HI(frame[offset+29]);

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int getFrame(uint16_t  *framePixelsBuffer, uint16_t numOfFrame, uintptr_t *deviceContextPtr);

/** \brief Clears memory

    \param[in] deviceStateStruct
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \details
    outReport[0]=7;

    inReport[0]=0x87;
    inReport[1]=errorCode;


    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int clearMemory(uintptr_t *deviceContextPtr);

/** \brief Erases user flash memory.
    \note There is no way to partially erase it.

    \param[in] deviceStateStruct
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \details
    outReport[0] = 0x1C;

    inReport[0] = 0x9C;
    inReport[1] = errorCode;

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int eraseFlash(uintptr_t *deviceContextPtr);

/** \brief Reads from user flash memory
    Reads bytesToRead from user flash memory starting at offset and copies them to the buffer
    \param[out] buffer - provide an initialized pointer to the buffer of unsigned char elements.
    \param[in] absoluteOffset
    \parblock
    Any value from 0 to 1FFFF*
    *you can read only 1 byte from 1FFFF, 2 bytes from 1FFFE etc.
    \endparblock

    \param[in] deviceStateStruct
    \parblock
    If the threadSafeMode is set to true, this pointer provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \details
    outReport[1] = 0x1A;
    outReport[2] = absoluteOffset[0];     //low byte
    outReport[3] = absoluteOffset[1];     //little endian!
    outReport[4] = absoluteOffset[2];
    outReport[5] = absoluteOffset[3];     //high byte
    outReport[6] = packets;

    inReport[0] = 0x9A;
    inReport[1] = LO(localOffset);
    inReport[2] = HI(localOffset);
    inReport[3] = packetsRemaining_or_IsError;
    inReport[4] = flash[absoluteOffset + localOffset];
    inReport[5] = flash[absoluteOffset + localOffset + 1];
    ..
    inReport[63] = flash[absoluteOffset + localOffset + 59];

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int readFlash(uint8_t *buffer, uint32_t absoluteOffset, uint32_t bytesToRead, uintptr_t *deviceContextPtr);

/** \brief Writes bytesToWrite bytes from the buffer to the user flash memory starting at offset
    \param[out] buffer
    \param[in] absoluteOffset
    \parblock
    offset can be any value from 0 to 1FFFF*
    *you can write only 1 byte to 1FFFF, 2 bytes to 1FFFE etc.

    you can write only to empty memory locations that have values = 0xff.
    The only way to get their state back to empty is erasing all user memory by eraseFlash(). There is no way to partially erase it.
    128kb available
    \endparblock
    \param[in] bytesToWrite

    \param[in] deviceContextPtr
    \parblock
    This parameter provides the device state information required for all the other functions.
    (Initialize this pointer with any of the connectToSingleDevice* functions and provide it to the other functions in this library to work in the thread-safe mode)
    If the requested device is already connected, disconnects and initializes the state again(in thread-safe mode)
    \endparblock

    \details
    outReport[1] = 0x1B;
    outReport[2] = offset[0];     //low byte
    outReport[3] = offset[1];     //little endian!
    outReport[4] = offset[2];
    outReport[5] = offset[3];     //high byte
    outReport[6] = numberOfbytes;     // <= 58 (max payload length)

    outReport[7] = bytesToWrite[0]; //payload begin
    outReport[8] = bytesToWrite[1];
    ...
    outReport[n] = bytesToWrite[m]; //payload end
    //n = numberOfbytes + 6, m = numberOfbytes - 1;

    inReport[0] = 0x9B;
    inReport[1] = errorCode;

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int writeFlash(uint8_t *buffer, uint32_t absoluteOffset, uint32_t bytesToWrite,  uintptr_t *deviceContextPtr);

/** \brief Resets all the device parameters to their default values and clears the memory
    \details
    default parameters:
    numOfStartElement = 0
    numOfEndElement = 3647
    reductionMode = 0
    numOfScans = 1
    numOfBlankScans = 0
    scanMode = 0
    exposureTime = 10

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int resetDevice(uintptr_t *deviceContextPtr);

/** \brief Disconnects the device
The device will be disconnected from USB, and any interaction with it will not be possible until the device is reset

    \ingroup API

    \returns
        This function returns 0 on success and error code in case of error.
*/
LIBSHARED_AND_STATIC_EXPORT int detachDevice(uintptr_t *deviceContextPtr);

#ifndef SPECTROMETER_ERROR_CODES
#define SPECTROMETER_ERROR_CODES
    #define OK 0
    #define CONNECT_ERROR_WRONG_ID 500
    #define CONNECT_ERROR_NOT_FOUND 501
    #define CONNECT_ERROR_FAILED 502
    #define DEVICE_NOT_INITIALIZED 503
    #define WRITING_PROCESS_FAILED 504
    #define READING_PROCESS_FAILED 505
    #define WRONG_ANSWER 506
    #define GET_FRAME_REMAINING_PACKETS_ERROR 507
    #define NUM_OF_PACKETS_IN_FRAME_ERROR 508
    #define INPUT_PARAMETER_NOT_INITIALIZED 509
    #define READ_FLASH_REMAINING_PACKETS_ERROR 510    

    #define CONNECT_ERROR_WRONG_SERIAL_NUMBER 516

    #define NO_DEVICE_CONTEXT_ERROR 585
#endif

#ifndef AS_CORE_LIBRARY
    #ifdef __cplusplus
        }
    #endif
#endif

#endif
