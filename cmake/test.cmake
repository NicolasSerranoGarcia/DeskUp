#create a flag that enables the test compilation

if(BUILD_TESTS)

    include(CTest)
    
    message(STATUS "Building tests is ENABLED")
    enable_testing()
    #add support for tests. Inside the CMakeLists.txt
    #inside test/, there are all the test modules.
    #using this structure, different flags can be defined
    #to only import certain tests to be run.
    
    #---add google tests with CPM---#
    
    include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

    CPMAddPackage(
        NAME GoogleTest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG v1.14.0
        OPTIONS "INSTALL_GTEST OFF"
        OPTIONS "BENCHMARK_DOWNLOAD_DEPENDENCIES=ON"
    )

    add_subdirectory(${CMAKE_SOURCE_DIR}/test)

else()
    message(STATUS "Building tests is DISABLED")
endif()

