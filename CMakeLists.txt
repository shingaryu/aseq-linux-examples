cmake_minimum_required (VERSION 3.6)
project (spectrometer_all)

option(WITHOUT_APPS "Don't build the example applications" OFF)
option(WITH_EXTRA_SYSTEM_LINKED_APPS "Show the apps that link dynamically after you install library in the system (/usr/local/lib by default)" ON)

###### packaging ######
if (UNIX)

    SET(INSTALL_PATH "${CMAKE_INSTALL_PREFIX}" CACHE PATH "Installation prefix")
    set(INSTALL_LIB_DIR       lib CACHE PATH "Installation directory for libraries")
    set(INSTALL_BIN_DIR       bin CACHE PATH "Installation directory for executables")
    set(INSTALL_INCLUDE_DIR   include CACHE PATH "Installation directory for header files")
    set(INSTALL_EXAMPLES_DIR  bin CACHE PATH "Installation directory for example files")

    set(MAIN_PACKAGE_NAME "spectrometer")

    set(CPACK_PACKAGE_VERSION_MAJOR "1")
    set(CPACK_PACKAGE_VERSION_MINOR "0")
    set(CPACK_PACKAGE_VERSION_PATCH "0")
    set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")   # For shared libraries and executables on Windows the VERSION attribute is parsed to extract a “major.minor” version number.

    set(SOVERSION "1")      #API version

    set(CPACK_GENERATOR "DEB")

    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
    endif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")

    set(CUSTOM_PACKAGE_SUFFIX "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Developer")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "")
    set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")

    set(CPACK_DEB_COMPONENT_INSTALL 1)  #allows to create multiple packages (e.g. main (unspecified COMPONENT) and dev)

    ############ main unnamed component ######################
    set(CPACK_DEBIAN_MAIN_PACKAGE_NAME "lib${MAIN_PACKAGE_NAME}")
    set(CPACK_DEBIAN_MAIN_PACKAGE_DEPENDS "udev")
    set(CPACK_DEBIAN_MAIN_DESCRIPTION "This library allows to work with **** spectrometers")
    set(CPACK_DEBIAN_MAIN_PACKAGE_SECTION "libs")
    set(CPACK_DEBIAN_MAIN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_MAIN_FILE_NAME "${CPACK_DEBIAN_MAIN_PACKAGE_NAME}_${CUSTOM_PACKAGE_SUFFIX}.deb")

    ########### dev component #######################
    set(CPACK_DEBIAN_DEV_PACKAGE_NAME "lib${MAIN_PACKAGE_NAME}-dev")
    set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_MAIN_PACKAGE_NAME}")
    set(CPACK_DEBIAN_DEV_DESCRIPTION "Contains headers and the static library for libspectrometer package")
    set(CPACK_DEBIAN_DEV_PACKAGE_SECTION "devel")
    set(CPACK_DEBIAN_DEV_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_DEV_FILE_NAME "${CPACK_DEBIAN_DEV_PACKAGE_NAME}_${CUSTOM_PACKAGE_SUFFIX}.deb")

    if (NOT WITHOUT_APPS)
        ########### examples component ####################
        set(CPACK_DEBIAN_EXAMPLES_PACKAGE_NAME "lib${MAIN_PACKAGE_NAME}-examples")
        set(CPACK_DEBIAN_EXAMPLES_PACKAGE_DEPENDS "${CPACK_DEBIAN_MAIN_PACKAGE_NAME}")
        set(CPACK_DEBIAN_EXAMPLES_DESCRIPTION "Contains examples for libspectrometer package")
        set(CPACK_DEBIAN_EXAMPLES_PACKAGE_SECTION "devel")
        set(CPACK_DEBIAN_EXAMPLES_PACKAGE_PRIORITY "extra")
        set(CPACK_DEBIAN_EXAMPLES_FILE_NAME "${CPACK_DEBIAN_EXAMPLES_PACKAGE_NAME}_${CUSTOM_PACKAGE_SUFFIX}.deb")
    endif(NOT WITHOUT_APPS)

    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/results/bin)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/results/lib)
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/results/lib)
    set(CPACK_OUTPUT_FILE_PREFIX "results/packages")
endif(UNIX)

add_subdirectory(library)

if (NOT WITHOUT_APPS)
   add_subdirectory(applications)
endif(NOT WITHOUT_APPS)

if (UNIX)
    include(CPack)
endif(UNIX)
