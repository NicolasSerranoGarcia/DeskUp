#include <benchmark/benchmark.h>

#include "window_core.h"
#include "desk_up_window_device.h"

// Benchmark device initialization
static void BM_CreateWindowDevice(benchmark::State& state) {
    for (auto _ : state) {
        DU_Init();
        benchmark::DoNotOptimize(current_window_backend);
        DU_Destroy();
    }
}

// Benchmark getting window geometry (X position)
static void BM_GetWindowXPos(benchmark::State& state) {
    DU_Init();
    if (!current_window_backend) {
        state.SkipWithError("Backend not initialized");
        return;
    }

    for (auto _ : state) {
        auto result = current_window_backend->getWindowXPos(current_window_backend.get());
        benchmark::DoNotOptimize(result);
    }

    DU_Destroy();
}

// Benchmark getting window geometry (Y position)
static void BM_GetWindowYPos(benchmark::State& state) {
    DU_Init();
    if (!current_window_backend) {
        state.SkipWithError("Backend not initialized");
        return;
    }

    for (auto _ : state) {
        auto result = current_window_backend->getWindowYPos(current_window_backend.get());
        benchmark::DoNotOptimize(result);
    }

    DU_Destroy();
}

// Benchmark getting window width
static void BM_GetWindowWidth(benchmark::State& state) {
    DU_Init();
    if (!current_window_backend) {
        state.SkipWithError("Backend not initialized");
        return;
    }

    for (auto _ : state) {
        auto result = current_window_backend->getWindowWidth(current_window_backend.get());
        benchmark::DoNotOptimize(result);
    }

    DU_Destroy();
}

// Benchmark getting window height
static void BM_GetWindowHeight(benchmark::State& state) {
    DU_Init();
    if (!current_window_backend) {
        state.SkipWithError("Backend not initialized");
        return;
    }

    for (auto _ : state) {
        auto result = current_window_backend->getWindowHeight(current_window_backend.get());
        benchmark::DoNotOptimize(result);
    }

    DU_Destroy();
}

// Benchmark getting all geometry at once
static void BM_GetAllWindowGeometry(benchmark::State& state) {
    DU_Init();
    if (!current_window_backend) {
        state.SkipWithError("Backend not initialized");
        return;
    }

    for (auto _ : state) {
        auto x = current_window_backend->getWindowXPos(current_window_backend.get());
        auto y = current_window_backend->getWindowYPos(current_window_backend.get());
        auto w = current_window_backend->getWindowWidth(current_window_backend.get());
        auto h = current_window_backend->getWindowHeight(current_window_backend.get());
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
        benchmark::DoNotOptimize(w);
        benchmark::DoNotOptimize(h);
    }

    DU_Destroy();
}

// Benchmark getting path from window
static void BM_GetPathFromWindow(benchmark::State& state) {
    DU_Init();
    if (!current_window_backend) {
        state.SkipWithError("Backend not initialized");
        return;
    }

    for (auto _ : state) {
        auto result = current_window_backend->getPathFromWindow(current_window_backend.get());
        benchmark::DoNotOptimize(result);
    }

    DU_Destroy();
}

// Benchmark enumerating all open windows
static void BM_GetAllOpenWindows(benchmark::State& state) {
    DU_Init();
    if (!current_window_backend) {
        state.SkipWithError("Backend not initialized");
        return;
    }

    for (auto _ : state) {
        auto result = current_window_backend->getAllOpenWindows(current_window_backend.get());
        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }

    DU_Destroy();
}

// Benchmark getting DeskUp path
static void BM_GetDeskUpPath(benchmark::State& state) {
    DU_Init();
    if (!current_window_backend) {
        state.SkipWithError("Backend not initialized");
        return;
    }

    for (auto _ : state) {
        auto result = current_window_backend->getDeskUpPath();
        benchmark::DoNotOptimize(result);
    }

    DU_Destroy();
}

BENCHMARK(BM_CreateWindowDevice);
BENCHMARK(BM_GetWindowXPos);
BENCHMARK(BM_GetWindowYPos);
BENCHMARK(BM_GetWindowWidth);
BENCHMARK(BM_GetWindowHeight);
BENCHMARK(BM_GetAllWindowGeometry);
BENCHMARK(BM_GetPathFromWindow);
BENCHMARK(BM_GetAllOpenWindows);
BENCHMARK(BM_GetDeskUpPath);