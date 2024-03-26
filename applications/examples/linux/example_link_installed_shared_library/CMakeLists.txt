project("example-link-installed-shared-library")

FIND_SPECTROMETER()

include_directories(${SPECTROMETER_INCLUDE_DIR})

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} ${SPECTROMETER_LIBRARY})

set_target_properties(${PROJECT_NAME}
                        PROPERTIES
                        OUTPUT_NAME libspectrometer-${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH}/${INSTALL_EXAMPLES_DIR} COMPONENT examples)
