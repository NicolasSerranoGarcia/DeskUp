#include <benchmark/benchmark.h>

#include "desk_up_error_gui_converter.h"

static void BM_DeskUpErrorGuiConverter(benchmark::State& state) {
  for (auto _ : state){
    int i = 0;
    i = state.max_iterations;
  }
}

BENCHMARK(BM_DeskUpErrorGuiConverter);