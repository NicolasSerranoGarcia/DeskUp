# source/desk_up_window/CMakeLists.txt

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils
    ${CMAKE_BINARY_DIR}/source/desk_up_window_backend/backend_utils
)

add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_desc
                 ${CMAKE_BINARY_DIR}/source/desk_up_window_backend/window_desc)

add_library(desk_up_window_library)

target_sources(desk_up_window_library PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/desk_up_window.cc
    ${CMAKE_CURRENT_LIST_DIR}/desk_up_window.h
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core.cc
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_core.h
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_window_device.h
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_window_bootstrap.h
)

target_include_directories(desk_up_window_library PUBLIC
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
    ${CMAKE_SOURCE_DIR}/source/desk_up_error
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_backends/desk_up_win
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_desc
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/backend_utils
    ${CMAKE_SOURCE_DIR}/source/desk_up_window         # por si incluyes "desk_up_window.h"
    ${CMAKE_SOURCE_DIR}/include                 # si usas headers globales
)

# --- Subproyectos que compilan ---
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_backends/desk_up_win
    ${CMAKE_BINARY_DIR}/source/desk_up_window_backend/window_backends/desk_up_win
)

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/source/desk_up_error
    ${CMAKE_BINARY_DIR}/source/desk_up_error
)

# Enlazados. Si desk_up_win_library ya "PUBLIC-linkea" window_desc y error, bastaría con desk_up_win_library.
# Si no estás seguro, enlázalos explícitamente:
target_link_libraries(desk_up_window_library
    PUBLIC
        backend_utils_library
        window_desc_library
        desk_up_win_library
        desk_up_error_library
)
