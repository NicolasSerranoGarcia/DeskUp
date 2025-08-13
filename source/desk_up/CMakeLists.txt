#---create a library to be included in other CMake files---#

add_library(desk_up_library STATIC
    desk_up.cc
    desk_up.h
)

target_include_directories(desk_up_library PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(desk_up_library PRIVATE ${wxWidgets_LIBRARIES})


target_include_directories(desk_up_library PRIVATE ${CMAKE_SOURCE_DIR}/source/desk_up_frame)

target_link_libraries(desk_up_library PRIVATE desk_up_frame_library)