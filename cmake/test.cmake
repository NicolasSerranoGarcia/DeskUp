# ./cmake/test.cmake

if(BUILD_TESTS)
    message(STATUS "(DU) Building tests is ENABLED")

    # --- Manage tests from ctest utility

        include(CTest)
        enable_testing()
    
    # --- Fetch google test

        include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

        CPMAddPackage(
            NAME GoogleTest
            GITHUB_REPOSITORY google/googletest
            GIT_TAG v1.14.0
            OPTIONS "INSTALL_GTEST OFF"
            OPTIONS "BENCHMARK_DOWNLOAD_DEPENDENCIES=ON"
        )

        include(GoogleTest)

        find_package(Threads REQUIRED)

    # --- Create an interface library for google_benchmarks dependencies ---

        add_library(google_test_library INTERFACE)

        target_link_libraries(google_test_library INTERFACE 
            Threads::Threads
            gtest_main
        )

    # --- Include all the benchmarks

        add_subdirectory(${CMAKE_SOURCE_DIR}/test)

else()
    message(STATUS "(DU) Building tests is DISABLED")
endif()

