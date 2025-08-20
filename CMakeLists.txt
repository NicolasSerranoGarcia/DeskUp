cmake_minimum_required(VERSION 3.5)

cmake_policy(SET CMP0135 NEW)

# --- Project ---
file(READ "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION_STRING)
project(DeskUp VERSION ${PROJECT_VERSION_STRING})

# --- Build output ---
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# --- Compiler flags ---
add_library(DeskUp-compiler-flags INTERFACE)
target_compile_features(DeskUp-compiler-flags INTERFACE cxx_std_17)

# GCC/Clang flags
target_compile_options(DeskUp-compiler-flags INTERFACE
    "$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,ARMClang,LCC>:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>"
    "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W3>"
)

# Link flags
if(WIN32)
    target_link_options(DeskUp-compiler-flags INTERFACE
        -static
        -static-libgcc
        -static-libstdc++
    )
elseif(UNIX AND NOT APPLE)
    target_link_options(DeskUp-compiler-flags INTERFACE
        -static-libgcc
        -static-libstdc++
    )

    set(wxWidgets_CONFIG_EXECUTABLE "$ENV{HOME}/wx-static/bin/wx-config")
endif()
    
# --- Executable ---
if (WIN32)
    add_executable(DeskUp WIN32 source/main.cc)
    enable_language(RC)
    target_sources(DeskUp PRIVATE ${CMAKE_SOURCE_DIR}/resources.rc)
else()
    add_executable(DeskUp source/main.cc)
endif()

# --- wxWidgets static ---

set(wxWidgets_USE_STATIC ON)
if(WIN32)
    set(wxRoot "$ENV{HOME}/wx-static")
    set(wxIncludeDir "${wxRoot}/include/wx-3.2")
    set(wxConfigIncludeDir "${wxRoot}/lib/wx/include/msw-unicode-static-3.2")
    set(wxLibDir "${wxRoot}/lib")
  
    foreach(tgt DeskUp desk_up_library desk_up_frame_library)
        if(TARGET ${tgt})
            target_include_directories(${tgt} PRIVATE
                ${wxIncludeDir}
                ${wxConfigIncludeDir}
            )
        endif()
    endforeach()

    set(wxWidgets_LIBRARIES
        ${wxLibDir}/libwx_baseu-3.2.a
        ${wxLibDir}/libwx_mswu_core-3.2.a
        ${wxLibDir}/libwx_mswu_net-3.2.a
        ${wxLibDir}/libwxpng-3.2.a
        ${wxLibDir}/libwxjpeg-3.2.a
        ${wxLibDir}/libwxtiff-3.2.a
        ${wxLibDir}/libwxregexu-3.2.a
        ${wxLibDir}/libwxexpat-3.2.a
        ${wxLibDir}/libwxzlib-3.2.a
    )
else()
    find_package(wxWidgets REQUIRED COMPONENTS core base)
    if (wxWidgets_USE_FILE)
        include(${wxWidgets_USE_FILE})
    endif()
endif()


#Add flags for tests, benchmarks and other tools

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/benchmark.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

# --- Sub-libraries (without linking wxWidgets) ---
add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up)
add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_frame)

# --- Compile definitions ---
target_compile_definitions(DeskUp PRIVATE wxWidgets_STATIC)
target_link_libraries(DeskUp PRIVATE DeskUp-compiler-flags)

# --- Link sub-libraries ---
target_link_libraries(DeskUp PRIVATE
    desk_up_library
    desk_up_frame_library
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
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(DeskUp PRIVATE NDEBUG)
endif()

# --- Installation ---
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install")
install(TARGETS DeskUp DESTINATION bin)

# --- CPack ---
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "TGZ")
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${DeskUp_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${DeskUp_VERSION_MINOR}")
include(CPack)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#doxygen

include(FetchContent)
FetchContent_Declare(
    doxygen-awesome-css
    URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(doxygen-awesome-css)

# Save the location the files were cloned into
# This allows us to get the path to doxygen-awesome.css
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

message(STATUS "AWESOME_CSS_DIR = ${AWESOME_CSS_DIR}")

# Generate the Doxyfile
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)