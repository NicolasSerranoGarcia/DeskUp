add_library(desk_up_error_gui_converter_library STATIC
    desk_up_error_gui_converter.h
    desk_up_error_gui_converter.cc
)

target_include_directories(desk_up_error_gui_converter_library
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils 
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
        ${CMAKE_SOURCE_DIR}/source/desk_up_error
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_desc
)

include(${wxWidgets_USE_FILE})
target_link_libraries(desk_up_error_gui_converter_library PRIVATE ${wxWidgets_LIBRARIES})