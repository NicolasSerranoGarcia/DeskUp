# ./source/desk_up_backend_interface/CMakeLists.txt

# desk_up_backend_interface_library

    add_library(desk_up_backend_interface_library STATIC
        desk_up_backend_interface.cc
        desk_up_backend_interface.h
    )

# Private dependencies

    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
        ${CMAKE_BINARY_DIR}/source/desk_up_window_backend
    )

	target_sources(desk_up_backend_interface_library PRIVATE
		${CMAKE_SOURCE_DIR}/source/desk_up_lookup_table/desk_up_lookup_table.cc
		${CMAKE_SOURCE_DIR}/source/exceptions/desk_up_file_not_open_exception/desk_up_file_not_open_exception.cc
		${CMAKE_SOURCE_DIR}/source/exceptions/desk_up_file_not_clear_exception/desk_up_file_not_clear_exception.cc
	)

# Include path

    target_include_directories(desk_up_backend_interface_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_error
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
        ${CMAKE_SOURCE_DIR}/include
    )

# Dependencies

    target_link_libraries(desk_up_backend_interface_library PUBLIC
        config_compiler_flags_library

        desk_up_error_library
        desk_up_window_backend_library
    )
