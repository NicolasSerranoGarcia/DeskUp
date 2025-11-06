# ./source/desk_up_gui_error_converter/CMakeLists.txt

# desk_up_error_gui_converter_library

    add_library(desk_up_error_gui_converter_library STATIC
        desk_up_error_gui_converter.h
        desk_up_error_gui_converter.cc
    )

# Include path

    target_include_directories(desk_up_error_gui_converter_library PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_error
    )

# Dependencies

    target_link_libraries(desk_up_error_gui_converter_library PRIVATE 
        config_compiler_flags_library
        desk_up_qt_library

        desk_up_error_library
    )