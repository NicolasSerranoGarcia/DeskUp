# ./source/desk_up_window_backends/window_core/CMakeLists.txt

# window_core_library

    add_library(window_core_library STATIC
        window_core.h
        window_core.cc
    )

# Private dependencies

    target_sources(window_core_library PRIVATE
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_window_device.h
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/desk_up_window_bootstrap.h
    )

	# --- Includes DESKUPDIR ---

		add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_dir
				${CMAKE_BINARY_DIR}/source/desk_up_dir
		)

    if(WIN32)
        add_subdirectory(${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_backends/desk_up_win
            ${CMAKE_BINARY_DIR}/source/desk_up_window_backend/window_backends/desk_up_win
        )
    endif()

# Include path

    target_include_directories(window_core_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend
        ${CMAKE_SOURCE_DIR}/source/desk_up_window_backend/window_backends/desk_up_win
    )

# Dependencies

    target_link_libraries(window_core_library PUBLIC
        config_compiler_flags_library

		desk_up_dir_library
    )

    if(WIN32)
        target_link_libraries(window_core_library PUBLIC
            desk_up_win_library
        )
    endif()