# ./source/desk_up_window_backend/backend_utils/CMakeLists.txt

# backend_utils_library

    add_library(backend_utils_library STATIC
        backend_utils.cc
        backend_utils.h
    )

# Include path

    target_include_directories(backend_utils_library PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    )

# Dependencies

    target_link_libraries(backend_utils_library PUBLIC
        config_compiler_flags_library
    )

    if(WIN32)
        target_link_libraries(backend_utils_library PUBLIC
            desk_up_win32_library
        )
    endif()