#The window backend gets exported as a library created inside this CMakeLists.txt
add_library(desk_up_win_library STATIC)
target_sources(desk_up_win_library PUBLIC ${CMAKE_CURRENT_LIST_DIR}/desk_up_win.cc 
                                            ${CMAKE_CURRENT_LIST_DIR}/desk_up_win.h)

#Make backend directories visible for this library
target_include_directories(desk_up_win_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
                                                        ${CMAKE_SOURCE_DIR}/include
                                                        ${CMAKE_SOURCE_DIR}/source/desk_up_error
                                                        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_desc)

#Link against the abstract window_desc struct library located in ../window_desc from this file
target_link_libraries(desk_up_win_library PUBLIC window_desc_library)

#Link against Win32 API dll libraries
target_link_libraries(desk_up_win_library PUBLIC 
    user32
    gdi32
    kernel32
    advapi32
    comdlg32
    shell32)