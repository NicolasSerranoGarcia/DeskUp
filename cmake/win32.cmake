# Unlike other cmake modules, this one just defines the win32 interface library

    # --- Create an interface library representing win32. Any sub-library that uses win32 should link against this one ---

        add_library(desk_up_win32_library INTERFACE)

        target_compile_definitions(desk_up_win32_library INTERFACE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            _UNICODE
    		UNICODE
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
