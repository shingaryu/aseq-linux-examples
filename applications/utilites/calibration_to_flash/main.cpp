#include <iostream>
#include <fstream>

#include <vector>
#include <cstdlib>
#include "libspectrometer.h"


using namespace std;

int main(int argc, char *argv[])
{
    std::ifstream ifs("Calibration_1398.txt", std::ios_base::binary);

    if(!ifs) {
        std::cout << "Failed to open the calibration file. Exiting..." << std::endl;
        return EXIT_FAILURE;
    };

    ifs.seekg(0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg(0, ifs.beg);

    std::vector<unsigned char> fileBytes;
    fileBytes.resize(length);

    ifs.read(reinterpret_cast<char*>(fileBytes.data()), length);
    ifs.close();

    uintptr_t deviceHandle = 0;
    int result = connectToDeviceByIndex(0, &deviceHandle);

    if (result) {
        std::cout << "failed to connect the device, error: " << result << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<unsigned char> readBuffer;
    readBuffer.resize(length);

    result = eraseFlash(&deviceHandle);
    if (result) {
        std::cout << "failed to clear memory, error: " << result << std::endl;
        return EXIT_FAILURE;
    }

    result = readFlash(readBuffer.data(), 0, length, &deviceHandle);

    if (result) {
        std::cout << "failed to write to flash, error: " << result << std::endl;
        return EXIT_FAILURE;
    }

    result = writeFlash(fileBytes.data(), 0, length, &deviceHandle);

    if (result) {
        std::cout << "failed to write to flash, error: " << result << std::endl;
        return EXIT_FAILURE;
    }

    result = readFlash(readBuffer.data(), 0, length, &deviceHandle);

    if (result) {
        std::cout << "failed to write to flash, error: " << result << std::endl;
        return EXIT_FAILURE;
    }

    result = disconnectDeviceContext(&deviceHandle);


    std::ofstream readFlash("readFlash.txt");

    if (!readFlash.is_open()) {
        return EXIT_FAILURE;
    }

    #if defined(_WIN32)
        readFlash << "\r\n";
    #else
        readFlash << "\n";
    #endif

    for (const auto& element: readBuffer) {
        readFlash << element;
    }

    readFlash.close();    

    return EXIT_SUCCESS;
}
