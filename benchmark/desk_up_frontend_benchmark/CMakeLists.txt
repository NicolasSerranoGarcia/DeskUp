# ./benchmark/desk_up_frontend_benchmark/CMakeLists.txt

# desk_up_frontend_benchmark_library

    add_library(desk_up_frontend_benchmark_library OBJECT desk_up_frontend_benchmark.cc)

# Include path

    target_include_directories(desk_up_frontend_benchmark_library PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/source/desk_up_frontend
    )

# Dependencies

    target_link_libraries(desk_up_frontend_benchmark_library PUBLIC
        config_compiler_flags_library
        google_benchmark_library

        desk_up_frontend_library
    )