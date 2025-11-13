#include <benchmark/benchmark.h>

#include "mainWindow.h"

static void BM_DeskUpFrontend(benchmark::State& state) {
  for (auto _ : state){
    int i = 0;
    i = state.max_iterations;
  }
}

BENCHMARK(BM_DeskUpFrontend);