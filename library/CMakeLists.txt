project(spectrometer_libraries)

cmake_minimum_required(VERSION 2.8.6)

INCLUDE_DIRECTORIES("headers")

FILE(GLOB CORE_LIBRARY_HEADERS   "headers/hidapi.h"
                                 "headers/internal.h"
                                 "headers/libspectrometer.h"
                                 "headers/stdbool.h")

FILE(GLOB CORE_LIBRARY_SRC "src/internal.c"
                           "src/libspectrometer.c")

IF(WIN32)
	FILE(GLOB HIDAPI_SRC "src/windows/hid.c")
ELSE(WIN32)
	FILE(GLOB HIDAPI_SRC "src/linux/hid.c")
ENDIF(WIN32)

SET(CORE_SRCS ${CORE_LIBRARY_HEADERS} ${CORE_LIBRARY_SRC} ${HIDAPI_SRC})

SET(SPECTROMETER_LIBRARIES "")

# instructions for the static library #

#    if (LIB_STATIC_PIC)
#        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#    endif()
#
    add_library(spectrometer STATIC ${CORE_SRCS})
    list(APPEND SPECTROMETER_LIBRARIES spectrometer)
    set_target_properties(spectrometer  PROPERTIES
                                        OUTPUT_NAME spectrometer_static) #changing static library name

# instructions for the shared library #
    add_library(spectrometer_shared SHARED ${CORE_SRCS})
    list(APPEND SPECTROMETER_LIBRARIES spectrometer_shared)
    set_target_properties(spectrometer_shared PROPERTIES
                                              OUTPUT_NAME spectrometer)   #not libspectrometer_shared, just libspectrometer
    if (WIN32)
        set_target_properties(spectrometer_shared PROPERTIES COMPILE_FLAGS -Dlibspectrometer_EXPORTS)
    endif(WIN32)


foreach (lib ${SPECTROMETER_LIBRARIES})
    if (WIN32)
        target_link_libraries(${lib} setupapi)
    elseif (UNIX)
        message(STATUS "linking ${lib} to rt and udev")
        set_target_properties(${lib} PROPERTIES SOVERSION ${SOVERSION}) #SOVERSION set in top CMakeLists file
        target_link_libraries(${lib} rt udev)  #librt is part of the GNU C Library, libudev is required by hidapi
    endif(WIN32)
endforeach()

if (UNIX)
    install(TARGETS spectrometer_shared DESTINATION ${INSTALL_PATH}/${INSTALL_LIB_DIR} COMPONENT main)
    install(TARGETS spectrometer DESTINATION ${INSTALL_PATH}/${INSTALL_LIB_DIR} COMPONENT dev)
    install(FILES headers/libspectrometer.h DESTINATION ${INSTALL_PATH}/${INSTALL_INCLUDE_DIR} COMPONENT dev)
endif(UNIX)

#NOTE: win multithreaded wrapper is deprecated for current version
#if(WIN32)
    #FILE(GLOB WRAPPER_HEADERS "headers/*.h")
    #FILE(GLOB WRAPPER_SRC "src/*.c")
    #SET(WRAPPER_SRCS ${WRAPPER_HEADERS} ${WRAPPER_SRC})
    #add_library(spectrlib_shared_wrapper SHARED ${WRAPPER_SRCS})
    #set_target_properties(spectometer_wrapper PROPERTIES COMPILE_FLAGS -DAS_CORE_LIBRARY)
#endif(WIN32)
