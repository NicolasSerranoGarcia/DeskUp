#---create a library to be included in other CMake files---#
add_library(desk_up_frame_library STATIC
    desk_up_frame.cc
    desk_up_frame.h
)

target_include_directories(desk_up_frame_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window
                                                        ${CMAKE_SOURCE_DIR}/source/desk_up_error
                                                        ${CMAKE_SOURCE_DIR}/include
                                                        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
                                                        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_desc
                                                        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils
                                                        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_backends/desk_up_win)

target_link_libraries(desk_up_frame_library PUBLIC window_desc_library
                                                    wxWidgetsDeps)

target_compile_definitions(desk_up_frame_library PUBLIC wxWidgets_STATIC)

target_include_directories(desk_up_frame_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${wxWidgets_INCLUDE_DIRS}
    ${CMAKE_CURRENT_LIST_DIR}
)