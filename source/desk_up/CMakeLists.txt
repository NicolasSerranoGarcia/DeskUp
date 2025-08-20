#---create a library to be included in other CMake files---#
add_library(desk_up_library STATIC
    desk_up.cc
    desk_up.h
)

if(WIN32)
    target_link_libraries(desk_up_library PUBLIC wxWidgetsDeps)
endif()

target_compile_definitions(desk_up_library PUBLIC wxWidgets_STATIC)

target_include_directories(desk_up_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${wxWidgets_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/source/desk_up_frame
)

target_link_libraries(desk_up_library PUBLIC
    desk_up_frame_library
)
