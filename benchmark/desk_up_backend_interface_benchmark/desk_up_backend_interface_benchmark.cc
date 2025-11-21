#include <benchmark/benchmark.h>

#include "desk_up_backend_interface.h"
#include "window_core.h"
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

// Benchmark workspace name validation
static void BM_IsWorkspaceValid(benchmark::State& state) {
    std::string validName = "MyWorkspace";
    std::string invalidName = "Invalid:Name*";
    
    for (auto _ : state) {
        bool result1 = DeskUpBackendInterface::isWorkspaceValid(validName);
        bool result2 = DeskUpBackendInterface::isWorkspaceValid(invalidName);
        benchmark::DoNotOptimize(result1);
        benchmark::DoNotOptimize(result2);
    }
}

// Benchmark checking if workspace exists
static void BM_ExistsWorkspace(benchmark::State& state) {
    DU_Init();
    std::string workspaceName = "BenchmarkWorkspace";
    
    for (auto _ : state) {
        bool result = DeskUpBackendInterface::existsWorkspace(workspaceName);
        benchmark::DoNotOptimize(result);
    }

    DU_Destroy();
}

// Benchmark checking if file exists
static void BM_ExistsFile(benchmark::State& state) {
    DU_Init();
    fs::path testPath = fs::temp_directory_path() / "benchmark_test.txt";
    
    for (auto _ : state) {
        bool result = DeskUpBackendInterface::existsFile(testPath);
        benchmark::DoNotOptimize(result);
    }

    DU_Destroy();
}

// Benchmark saving all windows (end-to-end)
static void BM_SaveAllWindowsLocal(benchmark::State& state) {
    DU_Init();
    std::string workspaceName = "BenchmarkSaveWorkspace";
    
    // Clean up before benchmarking
    DeskUpBackendInterface::removeWorkspace(workspaceName);
    
    for (auto _ : state) {
        state.PauseTiming();
        DeskUpBackendInterface::removeWorkspace(workspaceName);
        state.ResumeTiming();
        
        auto result = DeskUpBackendInterface::saveAllWindowsLocal(workspaceName);
        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }
    
    // Clean up after benchmark
    DeskUpBackendInterface::removeWorkspace(workspaceName);
    DU_Destroy();
}

// Benchmark restoring windows (end-to-end)
static void BM_RestoreWindows(benchmark::State& state) {
    DU_Init();
    std::string workspaceName = "BenchmarkRestoreWorkspace";
    
    // Setup: save current state
    DeskUpBackendInterface::removeWorkspace(workspaceName);
    auto saveResult = DeskUpBackendInterface::saveAllWindowsLocal(workspaceName);
    
    if (!saveResult) {
        state.SkipWithError("Failed to create workspace for restore benchmark");
        DU_Destroy();
        return;
    }
    
    for (auto _ : state) {
        auto result = DeskUpBackendInterface::restoreWindows(workspaceName);
        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }
    
    // Clean up
    DeskUpBackendInterface::removeWorkspace(workspaceName);
    DU_Destroy();
}

// Benchmark workspace removal
static void BM_RemoveWorkspace(benchmark::State& state) {
    DU_Init();
    std::string workspaceName = "BenchmarkRemoveWorkspace";
    
    for (auto _ : state) {
        state.PauseTiming();
        // Create workspace before each iteration
        DeskUpBackendInterface::saveAllWindowsLocal(workspaceName);
        state.ResumeTiming();
        
        int result = DeskUpBackendInterface::removeWorkspace(workspaceName);
        benchmark::DoNotOptimize(result);
    }

    DU_Destroy();
}

// Benchmark complete save-restore-remove cycle
static void BM_CompleteWorkspaceCycle(benchmark::State& state) {
    DU_Init();
    std::string workspaceName = "BenchmarkCycleWorkspace";
    
    for (auto _ : state) {
        // Save
        auto saveResult = DeskUpBackendInterface::saveAllWindowsLocal(workspaceName);
        benchmark::DoNotOptimize(saveResult);
        
        // Check existence
        bool exists = DeskUpBackendInterface::existsWorkspace(workspaceName);
        benchmark::DoNotOptimize(exists);
        
        // Restore
        auto restoreResult = DeskUpBackendInterface::restoreWindows(workspaceName);
        benchmark::DoNotOptimize(restoreResult);
        
        // Remove
        int removeResult = DeskUpBackendInterface::removeWorkspace(workspaceName);
        benchmark::DoNotOptimize(removeResult);
        
        benchmark::ClobberMemory();
    }

    DU_Destroy();
}

BENCHMARK(BM_IsWorkspaceValid);
BENCHMARK(BM_ExistsWorkspace);
BENCHMARK(BM_ExistsFile);
BENCHMARK(BM_SaveAllWindowsLocal);
BENCHMARK(BM_RestoreWindows);
BENCHMARK(BM_RemoveWorkspace);
BENCHMARK(BM_CompleteWorkspaceCycle);