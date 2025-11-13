#include <benchmark/benchmark.h>

#include "desk_up_backend_interface.h"

static void BM_DeskUpBackendInterface(benchmark::State& state) {
  for (auto _ : state){
    int i = 0;
    i = state.max_iterations;
  }
}

BENCHMARK(BM_DeskUpBackendInterface);