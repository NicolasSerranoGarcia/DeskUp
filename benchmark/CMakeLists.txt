# ./benchmark/CMakeLists.txt

# desk_up_benchmark

    add_executable(desk_up_benchmark benchmark_main.cc)

# Include path

    target_include_directories(desk_up_benchmark PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}   
    )

# Benchmarks

    # --- desk_up_frontend_benchmarks ---

    add_subdirectory(desk_up_frontend_benchmark)
    target_link_libraries(desk_up_benchmark PUBLIC $<TARGET_OBJECTS:desk_up_frontend_benchmark_library>)

    target_link_libraries(desk_up_benchmark PUBLIC desk_up_frontend_benchmark_library)

# Dependencies

    target_link_libraries(desk_up_benchmark PUBLIC
        config_compiler_flags_library
        google_benchmark_library
    )