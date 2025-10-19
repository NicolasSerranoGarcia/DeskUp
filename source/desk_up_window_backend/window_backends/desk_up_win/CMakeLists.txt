# source/.../desk_up_win/CMakeLists.txt

add_library(desk_up_win_library STATIC)

# Lo habitual: fuentes PRIVATE, headers PUBLIC (si quieres exponerlos a dependientes)
target_sources(desk_up_win_library
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/desk_up_win.cc
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/desk_up_win.h
)

# Incluye SOLO lo propio del backend; evita meter aquí rutas de otros módulos
target_include_directories(desk_up_win_library
    PUBLIC
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
        ${CMAKE_CURRENT_LIST_DIR}   # .../window_backends/desk_up_win
)

# Estas dependenicas deben venir del padre vía add_subdirectory(...)
target_link_libraries(desk_up_win_library
    PUBLIC
        window_desc_library
        desk_up_error_library
        user32
        gdi32
        kernel32
        advapi32
        comdlg32
        shell32
)
