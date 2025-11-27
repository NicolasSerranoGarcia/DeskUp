# ./source/desk_up_dir/CMakeLists.txt

# desk_up_dir_library

    add_library(desk_up_dir_library STATIC
        ${CMAKE_SOURCE_DIR}/include/desk_up_dir/desk_up_dir.h
        desk_up_dir.cc
    )

# Include path

    target_include_directories(desk_up_dir_library PUBLIC
		${CMAKE_CURRENT_LIST_DIR}
		${CMAKE_SOURCE_DIR}/include/
        ${CMAKE_SOURCE_DIR}/include/desk_up_dir/
    )

# Dependencies

    target_link_libraries(desk_up_dir_library PUBLIC
        config_compiler_flags_library
    )
