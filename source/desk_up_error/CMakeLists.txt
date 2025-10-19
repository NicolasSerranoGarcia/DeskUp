# source/desk_up_error/CMakeLists.txt
add_library(desk_up_error_library STATIC
    desk_up_error.cc
)

target_include_directories(desk_up_error_library
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}                    # .../source/desk_up_error
        ${CMAKE_SOURCE_DIR}/include                    # si backend_utils.h está aquí
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils                  # si backend_utils.h está aquí
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend  # si lo tuvieras ahí
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_desc  # si lo tuvieras ahí
)
