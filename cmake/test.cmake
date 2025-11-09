#create a flag that enables the test compilation

if(BUILD_TESTS)

    include(CTest)
    
    message(STATUS "(DU) Building tests is ENABLED")
    enable_testing()

    #---add google tests with CPM---#
    
    include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

    CPMAddPackage(
        NAME GoogleTest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG v1.14.0
        OPTIONS "INSTALL_GTEST OFF"
        OPTIONS "BENCHMARK_DOWNLOAD_DEPENDENCIES=ON"
    )

    find_package(Threads REQUIRED)

    add_library(google_test_library)

    target_link_libraries(google_test_library INTERFACE 
        Threads::Threads
        gtest_main
    )

    include(GoogleTest)

    add_subdirectory(${CMAKE_SOURCE_DIR}/test)

else()
    message(STATUS "(DU) Building tests is DISABLED")
endif()

