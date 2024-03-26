#include <stdio.h>
#include <stdlib.h>

#include "libspectrometer.h"

#define EXAMPLE_FLASH_BUFFER_BYTES 100

void exampleGetAndClearDevicesInfo()
{
    DeviceInfo_t *devices = getDevicesInfo();
    DeviceInfo_t *current = devices;

    uint16_t index = 0;

    while (current != NULL) {
        printf("HID device serial: %s (index: %d)\n", current->serialNumber, index++);
        current = current->next;
    }

    clearDevicesInfo(devices);  //should ALWAYS be called to free the allocated memory
}

void clearGlobals(uintptr_t* deviceHandle, uint8_t* buffer)
{
    printf("Disconnecting the device...");
    int result = disconnectDeviceContext(deviceHandle);
    if (result != OK) {
        printf("failed to disconnect the device, error: %d\n", result);
    } else {
        printf("success\n");
    }

    free(buffer);
}

int main(void)
{
    uintptr_t deviceHandle = 0;
    uint8_t* buffer = NULL;

    int count = getDevicesCount();
    printf("Number of devices: %d\n", count);

    if (!count) {
        printf("No devices found. Exiting...\n");
        return EXIT_SUCCESS;
    }

    exampleGetAndClearDevicesInfo();

    int result = connectToDeviceByIndex(0, &deviceHandle);

    if (result != OK) {
        printf("failed to connect the device, error: %d\n", result);
        return EXIT_FAILURE;
    }

    result = eraseFlash(&deviceHandle);
    if (result != OK) {
        printf("failed to clear memory, error: %d\n", result);
        clearGlobals(&deviceHandle, buffer);
        return EXIT_FAILURE;
    }

    buffer = (uint8_t*)calloc(EXAMPLE_FLASH_BUFFER_BYTES, sizeof(uint8_t));

    result = readFlash(buffer, 0, EXAMPLE_FLASH_BUFFER_BYTES, &deviceHandle);

    if (result != OK) {
        printf("failed to read flash, error: %d\n", result);
        clearGlobals(&deviceHandle, buffer);
        return EXIT_FAILURE;
    }

    result = writeFlash(buffer, 0, EXAMPLE_FLASH_BUFFER_BYTES, &deviceHandle);

    if (result != OK) {
        printf("failed to write to flash, error: %d\n", result);
        clearGlobals(&deviceHandle, buffer);
        return EXIT_FAILURE;
    }

    result = readFlash(buffer, 0, EXAMPLE_FLASH_BUFFER_BYTES, &deviceHandle);

    if (result) {
        printf("failed to read flash, error: %d\n", result);
        clearGlobals(&deviceHandle, buffer);
        return EXIT_FAILURE;
    }

    clearGlobals(&deviceHandle, buffer);

    return EXIT_SUCCESS;
}
