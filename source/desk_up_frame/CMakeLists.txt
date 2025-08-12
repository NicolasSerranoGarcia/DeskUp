#---create a library to be included in other CMake files---#

add_library(desk_up_frame_library STATIC
    desk_up_frame.cc
    desk_up_frame.h
)

target_include_directories(desk_up_frame_library PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)