add_library(window_desc_library STATIC)

target_include_directories(window_desc_library PUBLIC ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend)

target_sources(window_desc_library PUBLIC ${CMAKE_CURRENT_LIST_DIR}/window_desc.cc ${CMAKE_CURRENT_LIST_DIR}/window_desc.h)