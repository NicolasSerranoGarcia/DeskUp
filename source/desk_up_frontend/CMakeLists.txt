# ./source/desk_up_frontend/CMakeLists.txt

    add_library(desk_up_frontend_library STATIC
        main.cpp
        mainWindow.cpp
        mainWindow.h
        mainwindow.ui
    )

# Include path

    target_include_directories(desk_up_frontend_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_backend_interface
        ${CMAKE_SOURCE_DIR}/source/desk_up_error
        ${CMAKE_SOURCE_DIR}/source/desk_up_error_gui_converter
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core
    )

# Dependencies

    target_link_libraries(desk_up_frontend_library PUBLIC
        config_compiler_flags_library
        desk_up_qt_library

        window_desc_library
        desk_up_error_library
        desk_up_error_gui_converter_library
        window_core_library
        desk_up_backend_interface_library
    )

