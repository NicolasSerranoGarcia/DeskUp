# ./source/desk_up/CMakeLists.txt

# desk_up_library

    add_library(desk_up_library STATIC
        desk_up.cc
        desk_up.h
    )

# Include path

    target_include_directories(desk_up_library PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_frame
    )

# Dependencies

    target_link_libraries(desk_up_library PRIVATE 
        config_compiler_flags_library
        desk_up_wxWidgets_library

        desk_up_frame_library
    )
