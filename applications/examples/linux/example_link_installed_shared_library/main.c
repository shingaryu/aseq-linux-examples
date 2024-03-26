#include <stdio.h>
#include <stdlib.h>

#include "libspectrometer.h"

int main(void)
{
    uintptr_t deviceHandle = 0;

    int count = getDevicesCount();
    printf("Number of devices found: %d\n", count);

    if (!count) {
        printf("No devices found. Exiting...\n");
        return EXIT_SUCCESS;
    }

    printf("Found %d devices. Connecting to the first of them...", count);

    int result = connectToDeviceByIndex(0, &deviceHandle);
    if (result != OK) {
        printf("failed to connect the device, error: %d\n", result);
        return EXIT_FAILURE;
    } else {
        printf("success!\n");
    }

    printf("Disconnecting the device...");
    result = disconnectDeviceContext(&deviceHandle);
    if (result != OK) {
        printf("failed to disconnect the device, error: %d\n", result);
        return EXIT_FAILURE;
    } else {
        printf("success!\n");
    }

    return EXIT_SUCCESS;
}
