# ./test/desk_up_window_backend_test/CMakeLists.txt

# desk_up_window_backend_test

    add_executable(desk_up_window_backend_test desk_up_window_backend_test.cc)

# Include path

    target_include_directories(desk_up_window_backend_test PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
    )

# Dependencies

    target_link_libraries(desk_up_window_backend_test PUBLIC
        config_compiler_flags_library
        google_test_library

        desk_up_window_backend_library
    )

# Add to the test suite

    gtest_discover_tests(desk_up_window_backend_test)