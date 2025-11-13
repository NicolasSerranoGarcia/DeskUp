# ./test/desk_up_error_gui_converter_test/CMakeLists.txt

# desk_up_error_gui_converter_test

    add_executable(desk_up_error_gui_converter_test desk_up_error_gui_converter_test.cc)

# Include path

    target_include_directories(desk_up_error_gui_converter_test PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_error_gui_converter
    )

# Dependencies

    target_link_libraries(desk_up_error_gui_converter_test PUBLIC
        config_compiler_flags_library
        google_test_library

        desk_up_error_gui_converter_library
    )

# Add to the test suite

    gtest_discover_tests(desk_up_error_gui_converter_test)