# ./source/desk_up_lookup_table/CMakeLists.txt

# desk_up_workspace_lookup_table_library

    add_library(desk_up_workspace_lookup_table_library STATIC
		${CMAKE_SOURCE_DIR}/source/desk_up_workspace_lookup_table/desk_up_workspace_lookup_table.cc
		${CMAKE_SOURCE_DIR}/include/desk_up_workspace_lookup_table/desk_up_workspace_lookup_table.h
    )

# Private dependencies

	# --- Already includes the exceptions and the include paths of them

	add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_lookup_table
        ${CMAKE_BINARY_DIR}/source/desk_up_lookup_table
    )

# Include path

    target_include_directories(desk_up_workspace_lookup_table_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/desk_up_workspace_lookup_table
	)

# Dependencies

    target_link_libraries(desk_up_workspace_lookup_table_library PUBLIC
        config_compiler_flags_library

		desk_up_lookup_table_library
		# should have been defined already because of window_core. desk_up_workspace_lookup_table is used in the interface that is
		# defined before window_backend, which defines desk_up_dir inside window_core
		desk_up_dir_library
    )
