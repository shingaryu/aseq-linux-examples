project(utilite-calibration-to-flash)
cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SRC_LIST)

if (UNIX)
    FIND_SPECTROMETER()
    include_directories(${SPECTROMETER_INCLUDE_DIR})
else()
    include_directories(${CMAKE_SOURCE_DIR}/library/headers/ ${COMMON_HEADERS_DIR})  #comes from the main CMakeLists.txt
endif(UNIX)

add_executable(${PROJECT_NAME} ${SRC_LIST})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

#include(CheckCXXCompilerFlag)
#    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
#    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
#if(COMPILER_SUPPORTS_CXX11)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#elseif(COMPILER_SUPPORTS_CXX0X)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
#else()
#    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
#endif()


SET(REQUIRED_LIBS "")

if (UNIX)
    list(APPEND REQUIRED_LIBS ${SPECTROMETER_LIBRARY})
else()
    list(APPEND REQUIRED_LIBS "spectrometer_shared")
endif(UNIX)

target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS})


message(${PROJECT_SOURCE_DIR})
configure_file("${PROJECT_SOURCE_DIR}/Calibration_1398.txt" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Calibration_1398.txt" COPYONLY)
#add_dependencies(${PROJECT_NAME} "spectrometer_shared")

