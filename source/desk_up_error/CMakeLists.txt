# ./source/desk_up_error/CMakeLists.txt

# desk_up_error_library

    add_library(desk_up_error_library STATIC
        desk_up_error.h
        desk_up_error.cc
    )

# Private dependencies

    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils
        ${CMAKE_BINARY_DIR}/source/desk_up_window_backend/backend_utils
    )

    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_desc
        ${CMAKE_BINARY_DIR}/source/desk_up_window_backend/window_desc
    )

# Include path

    target_include_directories(desk_up_error_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}            
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_desc
    )

# Dependencies

    target_link_libraries(desk_up_error_library PRIVATE 
        config_compiler_flags_library

        window_desc_library
        backend_utils_library
    )

