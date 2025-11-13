# ./test/desk_up_backend_interface_test/CMakeLists.txt

# desk_up_backend_interface_test

    add_executable(desk_up_backend_interface_test desk_up_backend_interface_test.cc)

	target_sources(desk_up_backend_interface_test PUBLIC desk_up_dummy_device.h)

# Include path

    target_include_directories(desk_up_backend_interface_test PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_backend_interface
    )

# Dependencies

    target_link_libraries(desk_up_backend_interface_test PUBLIC
        config_compiler_flags_library
        google_test_library

        desk_up_backend_interface_library
    )

# Add to the test suite

    gtest_discover_tests(desk_up_backend_interface_test)