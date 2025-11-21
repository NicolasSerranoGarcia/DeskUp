#include <benchmark/benchmark.h>

#include "desk_up_error.h"
#ifdef _WIN32
    #include <Windows.h>
#endif

// Benchmark error construction
static void BM_ErrorConstruction(benchmark::State& state) {
    for (auto _ : state) {
        DeskUp::Error err(DeskUp::Level::Error, DeskUp::ErrType::InvalidInput, 0, "Test error message");
        benchmark::DoNotOptimize(err);
    }
}

// Benchmark error construction with move semantics
static void BM_ErrorConstructionMove(benchmark::State& state) {
    for (auto _ : state) {
        std::string msg = "Test error message with longer content for move semantics";
        DeskUp::Error err(DeskUp::Level::Fatal, DeskUp::ErrType::AccessDenied, 3, std::move(msg));
        benchmark::DoNotOptimize(err);
    }
}

// Benchmark error level checking
static void BM_ErrorLevelChecking(benchmark::State& state) {
    DeskUp::Error err(DeskUp::Level::Retry, DeskUp::ErrType::Io, 2, "Retry error");

    for (auto _ : state) {
        bool isFatal = err.isFatal();
        bool isRetryable = err.isRetryable();
        bool isSkippable = err.isSkippable();
        bool isWarning = err.isWarning();
        bool isError = err.isError();

        benchmark::DoNotOptimize(isFatal);
        benchmark::DoNotOptimize(isRetryable);
        benchmark::DoNotOptimize(isSkippable);
        benchmark::DoNotOptimize(isWarning);
        benchmark::DoNotOptimize(isError);
    }
}

// Benchmark error property access
static void BM_ErrorPropertyAccess(benchmark::State& state) {
    DeskUp::Error err(DeskUp::Level::Warning, DeskUp::ErrType::NotFound, 1, "Warning message");

    for (auto _ : state) {
        auto level = err.level();
        auto type = err.type();
        auto attempts = err.attempts();
        auto msg = err.what();

        benchmark::DoNotOptimize(level);
        benchmark::DoNotOptimize(type);
        benchmark::DoNotOptimize(attempts);
        benchmark::DoNotOptimize(msg);
    }
}

#ifdef _WIN32
// Benchmark Windows error code conversion
static void BM_FromLastWinError(benchmark::State& state) {
    for (auto _ : state) {
        SetLastError(ERROR_ACCESS_DENIED);
        auto err = DeskUp::Error::fromLastWinError("BM_FromLastWinError|test");
        benchmark::DoNotOptimize(err);
    }
}

// Benchmark Windows error with explicit code
static void BM_FromWinErrorCode(benchmark::State& state) {
    for (auto _ : state) {
        auto err = DeskUp::Error::fromLastWinError(ERROR_FILE_NOT_FOUND, "BM_FromWinErrorCode|test", 2);
        benchmark::DoNotOptimize(err);
    }
}

// Benchmark various Windows error codes
static void BM_FromWinErrorVariousCodes(benchmark::State& state) {
    DWORD codes[] = {
        ERROR_ACCESS_DENIED,
        ERROR_FILE_NOT_FOUND,
        ERROR_NOT_ENOUGH_MEMORY,
        ERROR_SHARING_VIOLATION,
        ERROR_DISK_FULL,
        ERROR_INVALID_HANDLE
    };

    size_t idx = 0;
    for (auto _ : state) {
        auto err = DeskUp::Error::fromLastWinError(codes[idx % 6], "BM_FromWinErrorVariousCodes|test");
        benchmark::DoNotOptimize(err);
        idx++;
    }
}
#endif

// Benchmark save error conversion
static void BM_FromSaveError(benchmark::State& state) {
    for (auto _ : state) {
        auto err = DeskUp::Error::fromSaveError(-3); // ERR_NO_PERMISSION
        benchmark::DoNotOptimize(err);
    }
}

// Benchmark save error various codes
static void BM_FromSaveErrorVariousCodes(benchmark::State& state) {
    int codes[] = { 1, -1, -2, -3, -4, -5, -6 };
    size_t idx = 0;

    for (auto _ : state) {
        auto err = DeskUp::Error::fromSaveError(codes[idx % 7]);
        benchmark::DoNotOptimize(err);
        idx++;
    }
}

// Benchmark Result<T> success path
static void BM_ResultSuccess(benchmark::State& state) {
    for (auto _ : state) {
        DeskUp::Result<int> result = 42;
        benchmark::DoNotOptimize(result);

        if (result.has_value()) {
            int value = result.value();
            benchmark::DoNotOptimize(value);
        }
    }
}

// Benchmark Result<T> error path
static void BM_ResultError(benchmark::State& state) {
    for (auto _ : state) {
        DeskUp::Result<int> result = std::unexpected(
            DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::NotFound, 0, "Not found")
        );
        benchmark::DoNotOptimize(result);

        if (!result.has_value()) {
            auto err = result.error();
            benchmark::DoNotOptimize(err);
        }
    }
}

// Benchmark Status success
static void BM_StatusSuccess(benchmark::State& state) {
    for (auto _ : state) {
        DeskUp::Status status{};
        benchmark::DoNotOptimize(status);

        bool success = status.has_value();
        benchmark::DoNotOptimize(success);
    }
}

// Benchmark Status error
static void BM_StatusError(benchmark::State& state) {
    for (auto _ : state) {
        DeskUp::Status status = std::unexpected(
            DeskUp::Error(DeskUp::Level::Fatal, DeskUp::ErrType::Io, 3, "IO error")
        );
        benchmark::DoNotOptimize(status);

        if (!status.has_value()) {
            auto err = status.error();
            benchmark::DoNotOptimize(err);
        }
    }
}

BENCHMARK(BM_ErrorConstruction);
BENCHMARK(BM_ErrorConstructionMove);
BENCHMARK(BM_ErrorLevelChecking);
BENCHMARK(BM_ErrorPropertyAccess);
#ifdef _WIN32
BENCHMARK(BM_FromLastWinError);
BENCHMARK(BM_FromWinErrorCode);
BENCHMARK(BM_FromWinErrorVariousCodes);
#endif
BENCHMARK(BM_FromSaveError);
BENCHMARK(BM_FromSaveErrorVariousCodes);
BENCHMARK(BM_ResultSuccess);
BENCHMARK(BM_ResultError);
BENCHMARK(BM_StatusSuccess);
BENCHMARK(BM_StatusError);