cmake_minimum_required (VERSION 3.6)
project (spectrometer_applications)

if(UNIX)
    include(${CMAKE_SOURCE_DIR}/cmake/FindSpectrometer.cmake)
endif(UNIX)

add_subdirectory(examples)


if (WITH_EXTRA_SYSTEM_LINKED_APPS)
   add_subdirectory(utilites)
   #specific utilites for testing/developing specific cases - in Unix it is linked to installed shared libs/headers only
endif()
