# ./source/desk_up_frame/CMakeLists.txt

# desk_up_frame_library

    add_library(desk_up_frame_library STATIC
        desk_up_frame.cc
        desk_up_frame.h
    )

# Include path

    target_include_directories(desk_up_frame_library PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_window
        ${CMAKE_SOURCE_DIR}/source/desk_up_error
        ${CMAKE_SOURCE_DIR}/source/desk_up_error_gui_converter
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core
    )

# Dependencies

    target_link_libraries(desk_up_frame_library PRIVATE 
        config_compiler_flags_library
        desk_up_wxWidgets_library

        window_desc_library
        desk_up_error_library
        desk_up_error_gui_converter_library
        window_core_library
        desk_up_window_library
    )