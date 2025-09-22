add_library(desk_up_window_library)

target_sources(desk_up_window_library PUBLIC ${CMAKE_CURRENT_LIST_DIR}/desk_up_window.h ${CMAKE_CURRENT_LIST_DIR}/desk_up_window.cc)

target_include_directories(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend)
target_include_directories(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_backends/desk_up_win)
target_include_directories(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/windowDesc)
target_include_directories(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_backends/desk_up_win
                 ${CMAKE_SOURCE_DIR}/build/source/desk_up_window_backend/window_backends/desk_up_win)

target_link_libraries(desk_up_window_library PUBLIC desk_up_win_library)

target_sources(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/include/backend_utils.h)

target_sources(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils.cc)
target_sources(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core.h)
target_sources(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core.cc)
target_sources(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_window_device.h)
target_sources(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_window_bootstrap.h)
target_sources(desk_up_window_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_is_available.h)