#---General---#

#set the minimum required version of CMake that other computers must use for this program
cmake_minimum_required(VERSION 3.16)

#Read the version from the file
file(READ "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION_STRING)
message(STATUS "Project version: ${PROJECT_VERSION_STRING}")

#set the project name
project(DeskUp VERSION ${PROJECT_VERSION_STRING})

#---Build location---#

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

#---Set compiler flags---#

#Create a flags library
add_library(DeskUp-compiler-flags INTERFACE)
#set the c++ standard to be C++13.
#Every library or executable that links this library will be required to have C++13
#Interface here represents that the attribute itself does not apply to the library, but to 
#the ones who link it
target_compile_features(DeskUp-compiler-flags INTERFACE cxx_std_13)

#create two variables to check which compile does the user have.
#In this case we are distinguishing between Linux/MacOS and Windows.
#This is useful because the flags from gcc are not the same used in MSVC
set(is-gcc-type-cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(is-msvc-cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

target_compile_options(DeskUp-compiler-flags INTERFACE 
                        "$<${is-gcc-type-cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
                        "$<${is-msvc-cxx}:$<BUILD_INTERFACE:-W3>>"
)

#---Add CPM installer from cmake/---#

include(cmake/CPM.cmake)

#---Add debug flags defined in tools.cmake---#

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

#---Add test enable flag defined in tests.cmake---#

#Note that this module does not add the tests itself, but the flag
#that does. So the tests are only accesible from this CMakeLists.txt
#when the flag BUILD_TESTING is set to true.
include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)

#---Add benchmark enable flag defined in benchmark.cmake---#

#Note that this module does not add the benchmark itself, but the flag
#that does. So the benchmark is only accessible from this CMakeLists.txt
#when the flag BUILD_BENCHMARK is set to true.
include(${CMAKE_SOURCE_DIR}/cmake/benchmark.cmake)

#---Create executable---#

#create the executable from the main function.
#This executable can the be linked with other libraries you have created.
add_executable(DeskUp
  source/main.cc
)

#---Add the option for choosing the release type. If the flag is not called, it is chosen in the terminal

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(DeskUp PRIVATE DEBUG_MODE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_definitions(DeskUp PRIVATE NDEBUG)
endif()

#---Link the project libraries---#
target_link_libraries(DeskUp INTERFACE DeskUp-compiler-flags)


add_subdirectory(source/desk_up)


#Here we are accessing the library defined in that CMakeLists.txt and linking it to the main executable 
target_link_libraries(DeskUp PRIVATE desk_up_library)

#---If there is a need to install (f.e CI/CD release)---#

#For example, let's suppose we want to send the executable to the source tree
#for testing purposes
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install")
#this will create a directory inside the root of the project called install, if it 
#is not created, and will place everything that is called with install() there.
#If the deployment is automated, the CI/CD tools will specifically create the package to release
#inside install, and then compress it to create a certain release

#---Install the executable inside the binary ---#

#As previously we changed the CMAKE_INSTALL_PREFIX env. variable to point to the install directory, when running
#cmake --install ., the DeskUp will go into install/bin
install(TARGETS DeskUp DESTINATION bin)

#Support for CPack

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ")
elseif(UNIX)
    set(CPACK_GENERATOR "TGZ")
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${CMakeTest_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CMakeTest_VERSION_MINOR}")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)