#---General---#

#set the minimum required version of CMake that other computers must use for this program
cmake_minimum_required(VERSION 3.25)

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

# Crear una librería de flags de compilación
add_library(DeskUp-compiler-flags INTERFACE)
target_compile_features(DeskUp-compiler-flags INTERFACE cxx_std_17)

# Detectar compilador
set(is-gcc-type-cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(is-msvc-cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

# Opciones de compilación
target_compile_options(DeskUp-compiler-flags INTERFACE 
                        "$<${is-gcc-type-cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused;-static-libgcc;-static-libstdc++>>"
                        "$<${is-msvc-cxx}:$<BUILD_INTERFACE:-W3>>"
)

#---add wxWidgets estático si disponible---#
find_package(wxWidgets REQUIRED COMPONENTS net core base)

if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

# Si quieres forzar wxWidgets estático:
# esto requiere tener wx-config apuntando a la versión estática
if(DEFINED ENV{WX_STATIC})
    set(wxWidgets_LIBRARIES "")
    execute_process(
        COMMAND wx-config --static --libs
        OUTPUT_VARIABLE WX_LIBS_STATIC
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(wxWidgets_LIBRARIES ${WX_LIBS_STATIC})
endif()

#---Create executable---#
add_executable(DeskUp
  source/main.cc
)

target_link_libraries(DeskUp PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(DeskUp INTERFACE DeskUp-compiler-flags)

#---Otras librerías internas---#
add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up)
target_link_libraries(DeskUp PRIVATE desk_up_library)

add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_frame)
target_link_libraries(DeskUp PRIVATE desk_up_frame_library)

#---Instalación---#
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install")
install(TARGETS DeskUp DESTINATION bin)
