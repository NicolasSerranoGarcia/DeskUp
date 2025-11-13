#include <benchmark/benchmark.h>

#include "desk_up_error.h"

static void BM_DeskUpError(benchmark::State& state) {
  for (auto _ : state){
    int i = 0;
    i = state.max_iterations;
  }
}

BENCHMARK(BM_DeskUpError);