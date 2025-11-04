# ./source/desk_up_window_backend/window_backends/desk_up_win/CMakeLists.txt

# desk_up_win_library

    add_library(desk_up_win_library STATIC
        ${CMAKE_CURRENT_LIST_DIR}/desk_up_win.cc
        ${CMAKE_CURRENT_LIST_DIR}/desk_up_win.h
    )

# Private dependencies

    target_sources(desk_up_win_library PRIVATE
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_window_device.h
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_window_bootstrap.h
    )

# Include path

    target_include_directories(desk_up_win_library PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_desc
        ${CMAKE_SOURCE_DIR}/source/desk_up_error
    )

# Dependencies

    target_link_libraries(desk_up_win_library PRIVATE
        config_compiler_flags_library
        desk_up_win32_library

        window_desc_library
        backend_utils_library
        desk_up_error_library
    )
