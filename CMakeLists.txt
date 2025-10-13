cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0077 NEW)

# --- Project ---
file(READ "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION_STRING)
project(DeskUp VERSION ${PROJECT_VERSION_STRING})

# --- Build output ---
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# --- Compiler flags ---
add_library(DeskUp-compiler-flags INTERFACE)
target_compile_features(DeskUp-compiler-flags INTERFACE cxx_std_23)

# GCC/Clang flags
target_compile_options(DeskUp-compiler-flags INTERFACE
    "$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,ARMClang,LCC>:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>"
    "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W3>"
)

# Link flags
target_link_options(DeskUp-compiler-flags INTERFACE
    -static
    -static-libgcc
    -static-libstdc++
)

# --- Executable ---
add_executable(DeskUp WIN32 source/main.cc source/desk_up_error/desk_up_error.h)
enable_language(RC)
target_sources(DeskUp PRIVATE ${CMAKE_SOURCE_DIR}/resources.rc)

add_compile_definitions(DESKUP_VERSION="${PROJECT_VERSION}")

# --- wxWidgets static ---

set(wxWidgets_USE_STATIC ON)
find_package(wxWidgets REQUIRED COMPONENTS net core base)

if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

message(STATUS "wx include dirs: ${wxWidgets_INCLUDE_DIRS}")
message(STATUS "wx libraries: ${wxWidgets_LIBRARIES}")

#Add flags for tests, benchmarks and other tools

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/benchmark.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/doxygen.cmake)

add_library(wxWidgetsDeps INTERFACE)
target_compile_definitions(wxWidgetsDeps INTERFACE wxWidgets_STATIC)
target_include_directories(wxWidgetsDeps INTERFACE ${wxWidgets_INCLUDE_DIRS})
target_link_libraries(wxWidgetsDeps INTERFACE ${wxWidgets_LIBRARIES})
target_link_libraries(wxWidgetsDeps INTERFACE
${wxWidgets_LIBRARIES})

# --- Sub-libraries (without linking wxWidgets) ---
add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up)
add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_frame)
add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window)
add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_error)

target_link_libraries(DeskUp PUBLIC
shlwapi
version
ole32
oleaut32
uuid
comctl32
winmm
ws2_32
gdi32
user32
kernel32)

target_link_libraries(DeskUp PUBLIC wxWidgetsDeps)

# --- Compile definitions ---
target_compile_definitions(DeskUp PRIVATE wxWidgets_STATIC)
target_link_libraries(DeskUp PRIVATE DeskUp-compiler-flags)

# --- Link sub-libraries ---
target_link_libraries(DeskUp PRIVATE
    desk_up_library
    desk_up_frame_library
    desk_up_window_library
    desk_up_error_library
    ${wxWidgets_LIBRARIES}
)

# --- Build type ---
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(DeskUp PRIVATE DEBUG_MODE)
    target_link_options(DeskUp PRIVATE -mconsole)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(DeskUp PRIVATE NDEBUG)
    target_link_options(DeskUp PRIVATE -mwindows)
endif()

# --- Installation ---
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install")
install(TARGETS DeskUp DESTINATION bin)

# --- CPack ---
set(CPACK_GENERATOR "ZIP")

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${DeskUp_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${DeskUp_VERSION_MINOR}")
include(CPack)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)