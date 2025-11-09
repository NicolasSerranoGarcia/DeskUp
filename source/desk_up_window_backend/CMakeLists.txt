# ./source/desk_up_window_backends/CMakeLists.txt

# desk_up_window_backend_library

    add_library(desk_up_window_backend_library STATIC)

# Private dependencies

    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core
        ${CMAKE_BINARY_DIR}/source/desk_up_window_backend/window_core
    )

# Include path

    target_include_directories(desk_up_window_backend_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(desk_up_window_backend_library PUBLIC
        config_compiler_flags_library

        window_core_library
    )