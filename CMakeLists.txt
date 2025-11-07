# ./CMakeLists.txt

# CMake config

    # --- CMake ---

        cmake_minimum_required(VERSION 3.20...3.25)

        set(CMAKE_CXX_STANDARD 23)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
        set(CMAKE_CXX_EXTENSIONS OFF)

    # --- Project ---

        file(READ "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION_STRING)
        project(
            DeskUp
            VERSION ${PROJECT_VERSION_STRING}
            DESCRIPTION "DeskUp — Workspace automation for Windows"
            HOMEPAGE_URL "https://github.com/NicolasSerranoGarcia/DeskUp"
            LANGUAGES CXX RC
        )

        # --- Some tools prefer the Major.Minor verison of SEMVER ---

        set(PROJECT_VERSION_SHORT "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

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
    add_executable(${PROJECT_NAME} WIN32 source/desk_up/main.cpp)

    # --- Resources file (currently just for the windows icon) ---
    enable_language(RC)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/resources.rc)

    target_link_libraries(${PROJECT_NAME} PRIVATE config_compiler_flags_library)

    # --- global preprocessor flag available when coding ---
    add_compile_definitions(DESKUP_VERSION="${PROJECT_VERSION}")

# qt

    # --- fetch qt ---

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
    
    # --- Create an interface library representing qt. Any sub-library that uses qt should link against this one --- 

    add_library(desk_up_qt_library INTERFACE)
    target_link_libraries(desk_up_qt_library INTERFACE Qt6::Core Qt6::Widgets)
    
    # --- Link it with the executable ---

    target_link_libraries(${PROJECT_NAME} PRIVATE desk_up_qt_library)

    # --- Acknowledge ---

    get_target_property(QT_CORE_INCLUDE_DIRS Qt6::Core INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(QT_WIDGETS_INCLUDE_DIRS Qt6::Widgets INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(QT_CORE_LIBS Qt6::Core LOCATION)
    get_target_property(QT_WIDGETS_LIBS Qt6::Widgets LOCATION)

    message(STATUS "(DU) Qt6 Core include dirs: ${QT_CORE_INCLUDE_DIRS}")
    message(STATUS "(DU) Qt6 Widgets include dirs: ${QT_WIDGETS_INCLUDE_DIRS}")
    message(STATUS "(DU) Qt6 Core lib: ${QT_CORE_LIBS}")
    message(STATUS "(DU) Qt6 Widgets lib: ${QT_WIDGETS_LIBS}")

# Windows API 

    # --- Create an interface library representing win32. Any sub-library that uses win32 should link against this one --- 

        add_library(desk_up_win32_library INTERFACE)

        target_compile_definitions(desk_up_win32_library INTERFACE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            _UNICODE
        )

        target_link_libraries(desk_up_win32_library INTERFACE
            shlwapi
            version
            ole32
            oleaut32
            uuid
            advapi32
            comdlg32
            shell32
            comctl32
            winmm
            ws2_32
            gdi32
            user32
            kernel32
        )

    # --- Link it with the executable ---

    target_link_libraries(${PROJECT_NAME} PRIVATE desk_up_win32_library)

# Auxiliary modules

    include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/benchmark.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/doxygen.cmake)

# Sub-libraries

    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_error)
    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_error_gui_converter)

    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_backend_interface) # Includes the backend libraries
    add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up)

# Link project libraries

    target_link_libraries(${PROJECT_NAME} PRIVATE
        desk_up_backend_interface_library
        desk_up_frontend_library
    )

# Set build type

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

# Installation

    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install")
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# CPack

    set(CPACK_GENERATOR "ZIP")

    include(InstallRequiredSystemLibraries)
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VENDOR "Nicolás Serrano García")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} automates your Windows workspace layout and window management.")
    set(CPACK_PACKAGE_CONTACT "serranogarcianicolas@gmail.com")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    include(CPack)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)