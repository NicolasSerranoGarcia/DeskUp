#include <benchmark/benchmark.h>

#include "window_core.h"

static void BM_DeskUpWindowBackend(benchmark::State& state) {
    for (auto _ : state){
        int i = 0;
        i = state.max_iterations;
    }
}

BENCHMARK(BM_DeskUpWindowBackend);