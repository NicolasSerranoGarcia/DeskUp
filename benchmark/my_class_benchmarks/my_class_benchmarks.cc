#include <benchmark/benchmark.h>

#include "my_class.h"

//Here we include the class we want to benchmark (in this case my_class) as a header file. 
//We use BENCHMARK() with a function call to measure the speed in which it finishes.

static void BM_ClassCreation(benchmark::State& state) {
  for (auto _ : state){
    MyClass newClass(2);
  }
}
// Register the function as a benchmark
BENCHMARK(BM_ClassCreation);


static void BM_Test(benchmark::State& state) {
  for (auto _ : state){
    int a = 3;
    int b = a%3;
  }
}
// Register the function as a benchmark
BENCHMARK(BM_Test);