# ./cmake/benchmark.cmake

if(BUILD_BENCHMARK)
    message(STATUS "(DU) Building benchmark is ENABLED")

    # --- fetch google benchmarks

        include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

        CPMAddPackage(
            NAME benchmark
            GITHUB_REPOSITORY google/benchmark
            GIT_TAG v1.9.4
            OPTIONS "BENCHMARK_ENABLE_TESTING OFF"
        )

    # --- Create an interface library for google_benchmarks dependencies ---

        add_library(google_benchmark_library INTERFACE)

        target_link_libraries(google_benchmark_library INTERFACE benchmark::benchmark)

    # --- Include all the benchmarks

        add_subdirectory(${CMAKE_SOURCE_DIR}/benchmark)

else()
    message(STATUS "(DU) Building benchmark is DISABLED")
endif()

