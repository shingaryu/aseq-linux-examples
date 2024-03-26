project(example_app_c_wrapper)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

include_directories(${CMAKE_SOURCE_DIR}/library/headers/)

add_executable(${PROJECT_NAME}  ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} spectrometer_wrapper)
