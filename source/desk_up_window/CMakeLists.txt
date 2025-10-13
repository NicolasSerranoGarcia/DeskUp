#The window interface gets exported as a library created inside this CMakeLists.txt
add_library(desk_up_window_library)
target_sources(desk_up_window_library PUBLIC ${CMAKE_CURRENT_LIST_DIR}/desk_up_window.h 
                                             ${CMAKE_CURRENT_LIST_DIR}/desk_up_window.cc
                                                ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core.h
                                                ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core.cc
                                                ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_window_device.h
                                                ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_window_bootstrap.h
                                                ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils.cc
                                                ${CMAKE_SOURCE_DIR}/include/backend_utils.h)

#Make backend directories visible for this library
target_include_directories(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
                                                            ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_backends/desk_up_win
                                                            ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/windowDesc
                                                            ${CMAKE_SOURCE_DIR}/source/desk_up_error
                                                            ${CMAKE_SOURCE_DIR}/include)

#Make directory with the window backend visible for this library
add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_backends/desk_up_win
                 ${CMAKE_SOURCE_DIR}/build/source/desk_up_window_backend/window_backends/desk_up_win)

#Link the window backend library against this one. Note that window_desc_library is linked against the own desk_up_win_library, so
#it is not necessary to link it again here
target_link_libraries(desk_up_window_library PUBLIC desk_up_win_library)