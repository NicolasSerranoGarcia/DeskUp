
if(BUILD_BENCHMARK)
    message(STATUS "Building benchmark is ENABLED")
    #add support for tests. Inside the CMakeLists.txt
    #inside test/, there are all the test modules.
    #using this structure, different flags can be defined
    #to only import certain tests to be run.
    

    #---Add google benchmark with CPM---#

    include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

    CPMAddPackage(
        NAME benchmark
        GITHUB_REPOSITORY google/benchmark
        GIT_TAG v1.7.1
        OPTIONS "BENCHMARK_ENABLE_TESTING OFF"
    )

    add_subdirectory(${CMAKE_SOURCE_DIR}/benchmark)

else()
    message(STATUS "Building benchmark is DISABLED")
endif()

