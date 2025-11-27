# ./source/desk_up_lookup_table/CMakeLists.txt

# desk_up_lookup_table_library

    add_library(desk_up_lookup_table_library STATIC
		${CMAKE_SOURCE_DIR}/source/desk_up_lookup_table/desk_up_lookup_table.cc
    )

# Private dependencies

	# --- As these are exceptions thrown by the class, the user of the library is going
	# to need to have the exception defined and linked in order to work with it ---

	target_sources(desk_up_lookup_table_library PUBLIC
		${CMAKE_SOURCE_DIR}/source/exceptions/desk_up_file_not_open_exception/desk_up_file_not_open_exception.cc
		${CMAKE_SOURCE_DIR}/source/exceptions/desk_up_file_not_clear_exception/desk_up_file_not_clear_exception.cc
	)

# Include path

    target_include_directories(desk_up_lookup_table_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/exceptions/desk_up_file_not_open_exception
        ${CMAKE_SOURCE_DIR}/include/exceptions/desk_up_file_not_clear_exception
        ${CMAKE_SOURCE_DIR}/include/desk_up_lookup_table
	)

# Dependencies

    target_link_libraries(desk_up_lookup_table_library PUBLIC
        config_compiler_flags_library
    )
