# ./source/desk_up_window_backend/window_desc/CMakeLists.txt

# window_desc_library

    add_library(window_desc_library STATIC
        window_desc.cc 
        window_desc.h
    )

# Include path

    target_include_directories(window_desc_library PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(window_desc_library PRIVATE 
        config_compiler_flags_library
    )