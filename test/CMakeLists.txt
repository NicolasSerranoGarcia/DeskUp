#External test CMakeLists.txt It is used to compile all the tests.
#Maybe flags can be added to only compile certain modules 
add_library(wxWidgets_link INTERFACE)
target_link_libraries(wxWidgets_link INTERFACE ${wxWidgets_LIBRARIES})
target_include_directories(wxWidgets_link INTERFACE ${wxWidgets_INCLUDE_DIRS})
target_compile_definitions(wxWidgets_link INTERFACE wxWidgets_STATIC)

add_subdirectory(./desk_up_test)
add_subdirectory(./desk_up_frame_test)