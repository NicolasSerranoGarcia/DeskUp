add_library(desk_up_win_library STATIC)


target_include_directories(desk_up_win_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend)

target_include_directories(desk_up_win_library PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_include_directories(desk_up_win_library PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_include_directories(desk_up_win_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/windowDesc)
target_link_libraries(desk_up_win_library PUBLIC window_desc_library)

target_sources(desk_up_win_library PUBLIC ${CMAKE_CURRENT_LIST_DIR}/desk_up_win.cc ${CMAKE_CURRENT_LIST_DIR}/desk_up_win.h)

target_link_libraries(desk_up_win_library PUBLIC 
    user32
    gdi32
    kernel32
    advapi32
    comdlg32
    shell32)
