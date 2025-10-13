#The window interface gets exported as a library created inside this CMakeLists.txt
add_library(desk_up_error_library INTERFACE)

#add the header
target_sources(desk_up_error_library INTERFACE ${CMAKE_SOURCE_DIR}/source/desk_up_error/desk_up_error.h)