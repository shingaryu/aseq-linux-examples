add_subdirectory(example_single_device)
add_subdirectory(example_multiple_devices)

if (WITH_EXTRA_SYSTEM_LINKED_APPS)
    add_subdirectory(example_link_installed_shared_library)
endif()
