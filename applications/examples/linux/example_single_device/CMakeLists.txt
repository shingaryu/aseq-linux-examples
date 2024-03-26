project("example-single-device")

include_directories(${CMAKE_SOURCE_DIR}/library/headers/)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})


#to link dynamically use "spectrometer_shared" instead of "spectrometer"
target_link_libraries(${PROJECT_NAME} spectrometer)
add_dependencies(${PROJECT_NAME} spectrometer)

set_target_properties(${PROJECT_NAME}
                        PROPERTIES
                        OUTPUT_NAME libspectrometer-${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH}/${INSTALL_EXAMPLES_DIR} COMPONENT examples)
