add_library(desk_up_win_library STATIC)

# here goes he equivalent for win32
# target_include_directories(desk_up_win_library PUBLIC ${X11_INCLUDE_DIR})
# target_link_libraries(desk_up_win_library PUBLIC ${X11_LIBRARIES})

target_sources(desk_up_win_library PUBLIC ${CMAKE_CURRENT_LIST_DIR}/desk_up_win.cc ${CMAKE_SOURCE_DIR}/desk_up_win.h)
