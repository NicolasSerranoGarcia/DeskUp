#include <benchmark/benchmark.h>

#include "mainWindow.h"
#include "desk_up_backend_interface.h"
#include "window_core.h"
#include <QApplication>
#include <QTimer>

// Note: GUI benchmarks are tricky because Qt requires an event loop.
// These benchmarks focus on non-GUI aspects and simulated operations.

// Benchmark MainWindow construction
static void BM_MainWindowConstruction(benchmark::State& state) {
    int argc = 0;
    char* argv[] = { nullptr };
    QApplication app(argc, argv);

    for (auto _ : state) {
        MainWindow window;
        benchmark::DoNotOptimize(window);
    }
}

// Benchmark workspace name validation (frontend utility)
static void BM_WorkspaceValidation(benchmark::State& state) {
    DU_Init();

    std::vector<std::string> workspaceNames = {
        "ValidWorkspace",
        "Another_Workspace",
        "workspace123",
        "Invalid:Name",
        "Invalid*Name",
        "Invalid?Name",
        ""
    };

    size_t idx = 0;
    for (auto _ : state) {
        bool isValid = DeskUpBackendInterface::isWorkspaceValid(workspaceNames[idx % workspaceNames.size()]);
        benchmark::DoNotOptimize(isValid);
        idx++;
    }

    DU_Destroy();
}

// Benchmark workspace existence check (common frontend operation)
static void BM_WorkspaceExistenceCheck(benchmark::State& state) {
    DU_Init();

    std::vector<std::string> workspaceNames = {
        "ExistingWorkspace",
        "NonExistingWorkspace",
        "AnotherWorkspace"
    };

    // Create one workspace for testing
    DeskUpBackendInterface::saveAllWindowsLocal(workspaceNames[0]);

    size_t idx = 0;
    for (auto _ : state) {
        bool exists = DeskUpBackendInterface::existsWorkspace(workspaceNames[idx % workspaceNames.size()]);
        benchmark::DoNotOptimize(exists);
        idx++;
    }

    // Clean up
    DeskUpBackendInterface::removeWorkspace(workspaceNames[0]);
    DU_Destroy();
}

// Benchmark simulated save workflow (what happens when user clicks "Save")
static void BM_SimulatedSaveWorkflow(benchmark::State& state) {
    DU_Init();
    std::string workspaceName = "FrontendBenchmarkWorkspace";

    for (auto _ : state) {
        state.PauseTiming();
        DeskUpBackendInterface::removeWorkspace(workspaceName);
        state.ResumeTiming();

        // Validate name
        bool isValid = DeskUpBackendInterface::isWorkspaceValid(workspaceName);
        benchmark::DoNotOptimize(isValid);

        if (isValid) {
            // Save workspace
            auto result = DeskUpBackendInterface::saveAllWindowsLocal(workspaceName);
            benchmark::DoNotOptimize(result);
        }

        benchmark::ClobberMemory();
    }

    DeskUpBackendInterface::removeWorkspace(workspaceName);
    DU_Destroy();
}

// Benchmark simulated restore workflow (what happens when user clicks "Restore")
static void BM_SimulatedRestoreWorkflow(benchmark::State& state) {
    DU_Init();
    std::string workspaceName = "FrontendRestoreBenchmark";

    // Setup: create a workspace to restore
    DeskUpBackendInterface::removeWorkspace(workspaceName);
    auto saveResult = DeskUpBackendInterface::saveAllWindowsLocal(workspaceName);

    if (!saveResult) {
        state.SkipWithError("Failed to create workspace for restore benchmark");
        DU_Destroy();
        return;
    }

    for (auto _ : state) {
        // Check if workspace exists
        bool exists = DeskUpBackendInterface::existsWorkspace(workspaceName);
        benchmark::DoNotOptimize(exists);

        if (exists) {
            // Restore workspace
            auto result = DeskUpBackendInterface::restoreWindows(workspaceName);
            benchmark::DoNotOptimize(result);
        }

        benchmark::ClobberMemory();
    }

    DeskUpBackendInterface::removeWorkspace(workspaceName);
    DU_Destroy();
}

// Benchmark multiple workspace operations (frontend stress test)
static void BM_MultipleWorkspaceOperations(benchmark::State& state) {
    DU_Init();

    std::vector<std::string> workspaceNames = {
        "Workspace1",
        "Workspace2",
        "Workspace3"
    };

    for (auto _ : state) {
        // Save multiple workspaces
        for (const auto& name : workspaceNames) {
            if (DeskUpBackendInterface::isWorkspaceValid(name)) {
                auto result = DeskUpBackendInterface::saveAllWindowsLocal(name);
                benchmark::DoNotOptimize(result);
            }
        }

        // Check their existence
        for (const auto& name : workspaceNames) {
            bool exists = DeskUpBackendInterface::existsWorkspace(name);
            benchmark::DoNotOptimize(exists);
        }

        // Remove them
        for (const auto& name : workspaceNames) {
            int removed = DeskUpBackendInterface::removeWorkspace(name);
            benchmark::DoNotOptimize(removed);
        }

        benchmark::ClobberMemory();
    }

    // Cleanup
    for (const auto& name : workspaceNames) {
        DeskUpBackendInterface::removeWorkspace(name);
    }

    DU_Destroy();
}

BENCHMARK(BM_MainWindowConstruction);
BENCHMARK(BM_WorkspaceValidation);
BENCHMARK(BM_WorkspaceExistenceCheck);
BENCHMARK(BM_SimulatedSaveWorkflow);
BENCHMARK(BM_SimulatedRestoreWorkflow);
BENCHMARK(BM_MultipleWorkspaceOperations);