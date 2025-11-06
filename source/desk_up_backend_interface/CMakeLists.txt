# ./source/desk_up_backend_interface/CMakeLists.txt

# desk_up_backend_interface_library

    add_library(desk_up_backend_interface_library STATIC
        desk_up_backend_interface.cc
        desk_up_backend_interface.h
    )

# Private dependencies

    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core
        ${CMAKE_BINARY_DIR}/source/desk_up_window_backend/window_core
    )

# Include path

    target_include_directories(desk_up_backend_interface_library PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_error
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core
    )

# Dependencies

    target_link_libraries(desk_up_backend_interface_library PRIVATE
        config_compiler_flags_library
        
        desk_up_error_library
        window_core_library
    )
