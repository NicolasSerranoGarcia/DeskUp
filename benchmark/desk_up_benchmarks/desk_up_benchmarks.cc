#include <benchmark/benchmark.h>

#include "desk_up.h"

//Here we include the class we want to benchmark (in this case my_class) as a header file. 
//We use BENCHMARK() with a function call to measure the speed in which it finishes.

static void BM_ClassCreation(benchmark::State& state) {
  for (auto _ : state){
    DeskUpApp app;
  }
}
// Register the function as a benchmark
BENCHMARK(BM_ClassCreation);