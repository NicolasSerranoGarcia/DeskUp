cmake_minimum_required(VERSION 3.20...3.25)

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

# config_compiler_flags_library library

    # --- Compiler flags ---
    add_library(config_compiler_flags_library INTERFACE)
    target_compile_features(config_compiler_flags_library INTERFACE cxx_std_23)

    # --- GCC/Clang flags ---
    target_compile_options(config_compiler_flags_library INTERFACE
        "$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,ARMClang,LCC>:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>"
        "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W3>"
    )

    # --- Link flags ---
    target_link_options(config_compiler_flags_library INTERFACE
        -static
        -static-libgcc
        -static-libstdc++
    )

# Executable

    # --- DeskUp ---
    add_executable(${PROJECT_NAME} WIN32 source/main.cc)

    # --- Resources file (currently just for the windows icon) ---
    enable_language(RC)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/resources.rc)

    target_link_libraries(${PROJECT_NAME} PRIVATE config_compiler_flags_library)

    # --- global preprocessor flag available when coding ---
    add_compile_definitions(DESKUP_VERSION="${PROJECT_VERSION}")

# wxWidgets

    # --- fetch wxWidgets

    # This flag needs to be set on before fetching wxWidgets
    # for the executable to find some symbols
    set(wxWidgets_USE_STATIC ON)
    find_package(wxWidgets REQUIRED COMPONENTS net core base)

    # --- Create an interface library representing wxWidgets. Any sub-library that uses wxWidgets should link against this one --- 

    add_library(desk_up_wxWidgets_library INTERFACE)
    target_compile_definitions(desk_up_wxWidgets_library INTERFACE wxWidgets_STATIC)
    target_compile_definitions(desk_up_wxWidgets_library INTERFACE ${wxWidgets_DEFINITIONS})
    target_include_directories(desk_up_wxWidgets_library INTERFACE ${wxWidgets_INCLUDE_DIRS})
    target_link_libraries(desk_up_wxWidgets_library INTERFACE ${wxWidgets_LIBRARIES})

    # --- Link it with the executable

    target_link_libraries(${PROJECT_NAME} PRIVATE desk_up_wxWidgets_library)

    # --- Acknowledge ---

    message(STATUS "(DU) wxWidgets include dirs: ${wxWidgets_INCLUDE_DIRS}")
    message(STATUS "(DU) wxWidgets libraries: ${wxWidgets_LIBRARIES}")

# Auxiliary modules

    include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/benchmark.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/doxygen.cmake)

# Sub-libraries

    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up)
    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_frame)
    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window)
    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_error_gui_converter)

# --- Windows API --- 

target_link_libraries(${PROJECT_NAME} PRIVATE
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

# --- Link sub-libraries ---

target_link_libraries(${PROJECT_NAME} PRIVATE  
  -Wl,--start-group
  desk_up_frame_library
  desk_up_window_library
  desk_up_win_library  
  desk_up_error_gui_converter_library 
  desk_up_error_library   
  backend_utils_library  
  window_desc_library   
  desk_up_library  
  -Wl,--end-group
${wxWidgets_LIBRARIES}
)

# --- Build type ---
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
message(STATUS "(DU) Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE)
    target_link_options(${PROJECT_NAME} PRIVATE -mconsole)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
endif()

# --- Installation ---
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install")
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# --- CPack ---
set(CPACK_GENERATOR "ZIP")

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${DeskUp_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${DeskUp_VERSION_MINOR}")
include(CPack)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)