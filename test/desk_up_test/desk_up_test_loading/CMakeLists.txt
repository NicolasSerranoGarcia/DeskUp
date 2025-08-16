#This file creates an executable for the specific .cc test file contained in the same folder,
#which will also once linked with google tests, so the executable that comes out of this CMakeLists.txt is
# ready to be run, independently of who calls it then
add_executable(desk_up_test_loading desk_up_test_loading.cc)

target_link_libraries(desk_up_test_loading PRIVATE wxWidgets_link DeskUp-compiler-flags)

#As the library is private, linking it on the main CMakeLists.txt with add_subdirectory()
#will not make the compiler know where the class is from this file. FOr this wee need to specify
#the path again
target_include_directories(desk_up_test_loading PUBLIC
${CMAKE_SOURCE_DIR}/source/desk_up
)


#We also need to access manually the dependency that the private class has on the public class.
#this is because the library that is created on the private class (for import purposes inside the
#own program) imports the public library privately, so this test must access it too
target_link_libraries(desk_up_test_loading PRIVATE desk_up_library)


#even though the program or the tests may not need Multithreading, in case in a future a threading related feature 
#is implemented, the tests are already loaded to work with threading. It may also happen that other dependencies might
#need threading when using them on the tests, so this is why this is included here
find_package(Threads REQUIRED)

target_link_libraries(desk_up_test_loading PRIVATE Threads::Threads)

#create a main entry point for each individual test, instead of creating a single main for every test of the class
target_link_libraries(desk_up_test_loading PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(desk_up_test_loading)