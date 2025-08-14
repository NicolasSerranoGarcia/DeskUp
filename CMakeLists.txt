#---General---#
cmake_minimum_required(VERSION 3.25)

# Read the version from the file
file(READ "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION_STRING)
message(STATUS "Project version: ${PROJECT_VERSION_STRING}")

# Set the project name
project(DeskUp VERSION ${PROJECT_VERSION_STRING})

#---Build location---#
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

#---Set compiler flags---#
# Create a compilation flags library
add_library(DeskUp-compiler-flags INTERFACE)
target_compile_features(DeskUp-compiler-flags INTERFACE cxx_std_17)

# Detect compiler
set(is-gcc-type-cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(is-msvc-cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

# Compilation options
target_compile_options(DeskUp-compiler-flags INTERFACE 
    "$<${is-gcc-type-cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused;-static-libgcc;-static-libstdc++>>"
    "$<${is-msvc-cxx}:$<BUILD_INTERFACE:-W3>>"
)

#---Add wxWidgets static if available---#
find_package(wxWidgets REQUIRED COMPONENTS net core base)

if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

# If you want to force static wxWidgets:
# This requires wx-config pointing to the static version
if(DEFINED ENV{WX_STATIC})
    set(wxWidgets_LIBRARIES "")
    execute_process(
        COMMAND wx-config --static --libs
        OUTPUT_VARIABLE WX_LIBS_STATIC
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(wxWidgets_LIBRARIES ${WX_LIBS_STATIC})
endif()

#---Add CPM installer from cmake/---#
include(cmake/CPM.cmake)

#---Add debug flags defined in tools.cmake---#
include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

#---Add test enable flag defined in tests.cmake---#
include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)

#---Add benchmark enable flag defined in benchmark.cmake---#
include(${CMAKE_SOURCE_DIR}/cmake/benchmark.cmake)

#---Create executable---#
add_executable(DeskUp
    source/main.cc
)

#---Compile statically---#
target_compile_definitions(DeskUp PRIVATE wxWidgets_STATIC)


#---Build type---#
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

#---Link the project libraries flags---#
target_link_libraries(DeskUp PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(DeskUp INTERFACE DeskUp-compiler-flags)

#---Dependencies of main.cc---#
add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up)
target_link_libraries(DeskUp PRIVATE desk_up_library)

add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_frame)
target_link_libraries(DeskUp PRIVATE desk_up_frame_library)

#---Installation---#
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install")
install(TARGETS DeskUp DESTINATION bin)

#---Support for CPack---#
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ")
elseif(UNIX)
    set(CPACK_GENERATOR "TGZ")
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${DeskUp_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${DeskUp_VERSION_MINOR}")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)