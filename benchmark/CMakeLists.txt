#create the executable for the benchmarks
add_executable(this_benchmark benchmark_main.cc)

target_link_libraries(this_benchmark PUBLIC wxWidgets_link)

#---Once: link google benchmarks with the executable. The rest of the benchmarks get the library from here---#

target_link_libraries(this_benchmark PUBLIC benchmark::benchmark)

#---Once for each benchmark folder (e.g. desk_up_benchmarks/)---#

#---1. desk_up_benchmarks---#

#link with benchmarks for desk_up
target_sources(this_benchmark PRIVATE desk_up_benchmarks/desk_up_benchmarks.cc)

#Make the folder where the desk_up_library is defined
target_include_directories(this_benchmark PRIVATE ${CMAKE_SOURCE_DIR}/source/desk_up)

#Link the library for the class
target_link_libraries(this_benchmark PRIVATE desk_up_library)

#---2. ...---#