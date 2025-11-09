#include <benchmark/benchmark.h>

#include <iostream>

#include "mainWindow.h"

static void BM_DeskUpFrontend(benchmark::State& state) {
  for (auto _ : state){
    int i = 0;
    i = state.max_iterations;
    std::cout << "Hello World!" << std::endl;
  }
}

BENCHMARK(BM_DeskUpFrontend);