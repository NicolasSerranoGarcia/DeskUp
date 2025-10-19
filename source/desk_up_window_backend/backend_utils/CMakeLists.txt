add_library(backend_utils_library STATIC
    ${CMAKE_CURRENT_LIST_DIR}/backend_utils.cc
    ${CMAKE_CURRENT_LIST_DIR}/backend_utils.h
)

target_include_directories(backend_utils_library PUBLIC
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
)