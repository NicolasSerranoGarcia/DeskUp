# source/desk_up_error/CMakeLists.txt
add_library(desk_up_error_library STATIC
    desk_up_error.cc
)

target_include_directories(desk_up_error_library
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}  
        ${CMAKE_SOURCE_DIR}/include                
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils         
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend 
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_desc
)
