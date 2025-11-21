#include <benchmark/benchmark.h>

#include "desk_up_error_gui_converter.h"
#include "desk_up_error.h"

using namespace DeskUp;
using namespace DeskUp::UI;

// Benchmark error level mapping
static void BM_MapLevel(benchmark::State& state) {
    for (auto _ : state) {
        auto result1 = ErrorAdapter::mapLevel(Level::Fatal);
        auto result2 = ErrorAdapter::mapLevel(Level::Error);
        auto result3 = ErrorAdapter::mapLevel(Level::Warning);
        auto result4 = ErrorAdapter::mapLevel(Level::Retry);
        auto result5 = ErrorAdapter::mapLevel(Level::Info);

        benchmark::DoNotOptimize(result1);
        benchmark::DoNotOptimize(result2);
        benchmark::DoNotOptimize(result3);
        benchmark::DoNotOptimize(result4);
        benchmark::DoNotOptimize(result5);
    }
}

// Benchmark getting user message
static void BM_GetUserMessage(benchmark::State& state) {
    Error err(Level::Error, ErrType::AccessDenied, 2, "Access denied error");

    for (auto _ : state) {
        QString msg = ErrorAdapter::getUserMessage(err);
        benchmark::DoNotOptimize(msg);
    }
}

// Benchmark getting user message for various error types
static void BM_GetUserMessageVariousTypes(benchmark::State& state) {
    std::vector<Error> errors = {
        Error(Level::Fatal, ErrType::InsufficientMemory, 0, "Out of memory"),
        Error(Level::Error, ErrType::FileNotFound, 1, "File not found"),
        Error(Level::Warning, ErrType::Timeout, 2, "Operation timed out"),
        Error(Level::Retry, ErrType::NetworkError, 3, "Network error"),
        Error(Level::Info, ErrType::None, 0, "Info message"),
        Error(Level::Skip, ErrType::InvalidInput, 1, "Invalid input")
    };

    size_t idx = 0;
    for (auto _ : state) {
        QString msg = ErrorAdapter::getUserMessage(errors[idx % errors.size()]);
        benchmark::DoNotOptimize(msg);
        idx++;
    }
}

// Benchmark complete error to message conversion
static void BM_ErrorToMessageConversion(benchmark::State& state) {
    for (auto _ : state) {
        Error err(Level::Warning, ErrType::DiskFull, 1, "Disk is full");

        QString msg = ErrorAdapter::getUserMessage(err);
        auto level = ErrorAdapter::mapLevel(err.level());

        benchmark::DoNotOptimize(msg);
        benchmark::DoNotOptimize(level);
    }
}

// Benchmark QString creation from std::string
static void BM_QStringConversion(benchmark::State& state) {
    Error err(Level::Error, ErrType::CorruptedData, 0, "Corrupted data detected");

    for (auto _ : state) {
        QString msg = QString::fromUtf8(err.what());
        benchmark::DoNotOptimize(msg);
    }
}

// Benchmark mapping all error levels
static void BM_MapAllLevels(benchmark::State& state) {
    std::vector<Level> levels = {
        Level::Fatal,
        Level::Error,
        Level::Warning,
        Level::Retry,
        Level::Info,
        Level::Debug,
        Level::Default,
        Level::Skip,
        Level::None
    };

    size_t idx = 0;
    for (auto _ : state) {
        auto result = ErrorAdapter::mapLevel(levels[idx % levels.size()]);
        benchmark::DoNotOptimize(result);
        idx++;
    }
}

// Benchmark batch error message generation
static void BM_BatchErrorMessages(benchmark::State& state) {
    std::vector<Error> errors;
    errors.reserve(10);

    for (int i = 0; i < 10; ++i) {
        errors.emplace_back(
            static_cast<Level>(i % 9),
            static_cast<ErrType>(i % 15),
            i,
            "Batch error message " + std::to_string(i)
        );
    }

    for (auto _ : state) {
        for (const auto& err : errors) {
            QString msg = ErrorAdapter::getUserMessage(err);
            benchmark::DoNotOptimize(msg);
        }
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_MapLevel);
BENCHMARK(BM_GetUserMessage);
BENCHMARK(BM_GetUserMessageVariousTypes);
BENCHMARK(BM_ErrorToMessageConversion);
BENCHMARK(BM_QStringConversion);
BENCHMARK(BM_MapAllLevels);
BENCHMARK(BM_BatchErrorMessages);