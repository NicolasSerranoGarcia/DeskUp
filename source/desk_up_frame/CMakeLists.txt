#---create a library to be included in other CMake files---#
add_library(desk_up_frame_library STATIC
    desk_up_frame.cc
    desk_up_frame.h
)

if(WIN32)
    target_link_libraries(desk_up_frame_library PUBLIC wxWidgetsDeps)
endif()

target_include_directories(desk_up_frame_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window)
target_include_directories(desk_up_frame_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend)
target_include_directories(desk_up_frame_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/windowDesc)
target_include_directories(desk_up_frame_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_backends/desk_up_win)

add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/windowDesc ${CMAKE_SOURCE_DIR}/build/source/desk_up_window_backend/windowDesc)

target_link_libraries(desk_up_frame_library PUBLIC window_desc_library)

target_include_directories(desk_up_frame_library PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(desk_up_frame_library PUBLIC wxWidgets_STATIC)

target_include_directories(desk_up_frame_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${wxWidgets_INCLUDE_DIRS}
)