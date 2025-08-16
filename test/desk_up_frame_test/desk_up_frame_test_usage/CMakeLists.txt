#This file creates an executable for the specific .cc test file contained in the same folder,
#which will also once linked with google tests, so the executable that comes out of this CMakeLists.txt is
# ready to be run, independently of who calls it then
add_executable(desk_up_frame_test_usage desk_up_frame_test_usage.cc)

target_link_libraries(desk_up_frame_test_usage PRIVATE wxWidgets_link DeskUp-compiler-flags)

target_include_directories(desk_up_frame_test_usage PUBLIC
${CMAKE_SOURCE_DIR}/source/desk_up_frame_library
)

#Access the library created by the CMakeLists.txt inside the private class directory
target_link_libraries(desk_up_frame_test_usage PRIVATE desk_up_frame_library)

#even though the program or the tests may not need Multithreading, in case in a future a threading related feature 
#is implemented, the tests are already loaded to work with threading. It may also happen that other dependencies might
#need threading when using them on the tests, so this is why this is included here
find_package(Threads REQUIRED)

target_link_libraries(desk_up_frame_test_usage PRIVATE Threads::Threads)

#create a main entry point for each individual test, instead of creating a single main for every test of the class
target_link_libraries(desk_up_frame_test_usage PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(desk_up_frame_test_usage)