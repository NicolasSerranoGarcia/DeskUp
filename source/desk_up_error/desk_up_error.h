/**
 * @file desk_up_error.h
 * @brief Error management system for DeskUp (centralized error representation and conversion utilities).
 *
 * This file is part of DeskUp
 *
 * @author
 *   Nicolas Serrano Garcia <serranogarcianicolas@gmail.com>
 * @date
 *   2025
 * @copyright
 *   Copyright (C) 2025 Nicolas Serrano Garcia
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DESKUPERROR_H
#define DESKUPERROR_H

#include <string>
#include <stdexcept>
#include <optional>
#include <expected>
#ifdef _WIN32
    #include <Windows.h>
#endif

#include "window_desc.h"
#include "backend_utils.h"

namespace DeskUp {

    /**
     * @enum Level
     * @brief Represents the severity of an error.
     *
     * @details
     * This enumeration provides a consistent way to classify errors by their
     * impact and intended handling behavior.  
     * 
     * - **Fatal** → unrecoverable; execution must stop.  
     * - **Error** → serious failure; operation aborted.  
     * - **Warning** → non-critical issue; user notification recommended.  
     * - **Retry** → recoverable error that may succeed upon retry.  
     * - **Info** → informational message, not an error.  
     * - **Debug** → debug-only diagnostic message.  
     * - **Default** → unspecified severity.  
     * - **None** → represents the absence of error.  
     *
     * @see DeskUp::Error
     * @version 0.2.1
     * @date 2025
     */
    enum class Level {
        Fatal,
        Error,
        Warning,
        Retry,
        Info,
        Debug,
        Default,
        None
    };

    /**
     * @enum ErrType
     * @brief Represents the underlying type or origin of an error.
     *
     * @details
     * Each enumerator identifies a distinct class of errors so that DeskUp
     * can map platform or library failures into portable internal types.
     *
     * @see DeskUp::Error
     * @version 0.2.1
     * @date 2025
     */
    enum class ErrType {
        InsufficientMemory,   /**< Memory allocation failed or system out of resources. */
        AccessDenied,         /**< Permission denied by the OS or file system. */
        SharingViolation,     /**< Another process holds an exclusive lock. */
        Io,                   /**< Generic input/output error. */
        NotFound,             /**< Resource not found. */
        DiskFull,             /**< Storage volume full. */
        DeviceNotFound,       /**< Device unavailable or disconnected. */
        Timeout,              /**< Operation timed out. */
        ResourceBusy,         /**< Resource is in use. */
        FileNotFound,         /**< File could not be located. */
        InvalidFormat,        /**< Invalid file or data format. */
        InvalidInput,         /**< Invalid parameter passed by the caller. */
        CorruptedData,        /**< Data corruption detected. */
        OutOfRange,           /**< Index or parameter out of valid range. */
        NetworkError,         /**< Generic network failure. */
        ConnectionRefused,    /**< Connection attempt refused. */
        HostUnreachable,      /**< Target host cannot be reached. */
        ProtocolError,        /**< Violation of expected protocol behavior. */
        Unexpected,           /**< Unexpected runtime condition. */
        NotImplemented,       /**< Feature not yet implemented. */
        Default,              /**< Unspecified error type. */
        None                  /**< Represents no error. */
    };

    /**
     * @class Error
     * @brief Centralized representation of a DeskUp runtime error.
     *
     * @details
     * The `Error` class extends `std::runtime_error` to attach
     * structured metadata to exceptions thrown across DeskUp subsystems.
     * Each error carries:
     * - A **Level** (severity classification)
     * - An **ErrType** (category)
     * - The number of **retries** attempted (if applicable)
     * - An optional **windowDesc** structure indicating the affected window
     *
     * Conversion utilities (`fromLastWinError()`, `fromSaveError()`) map
     * Windows system errors and DeskUp-specific codes to structured
     * `Error` instances.
     *
     * @see Level
     * @see ErrType
     * @see windowDesc
     * @version 0.2.1
     * @date 2025
     */
    class Error final : public std::runtime_error {
    public:
        /**
         * @brief Default constructor (represents a non-error).
         *
         * @version 0.2.1
         * @date 2025
         */
        Error() : std::runtime_error(""), lvl(Level::None), errType(ErrType::None), retries(0) {}

        /**
         * @brief Constructs an error with full metadata.
         *
         * @param l Error level (severity).
         * @param err Error type (category).
         * @param t Number of retries attempted before failure.
         * @param msg Descriptive error message.
         * @version 0.2.1
         * @date 2025
         */
        Error(Level l, ErrType err, unsigned int t, std::string msg)
            : std::runtime_error(std::move(msg)), lvl(l), errType(err), retries(t) {}

        /**
         * @brief Returns the error type.
         * @return The associated @ref ErrType.
         */
        ErrType type() const noexcept { return errType; }

        /**
         * @brief Returns the error severity level.
         * @return The associated @ref Level.
         */
        Level level() const noexcept { return lvl; }

        /**
         * @brief Returns how many times the operation was retried.
         * @return Integer representing the retry count.
         */
        int attempts() const noexcept { return retries; }

        /**
         * @brief Returns the affected window (if available).
         * @return A @ref windowDesc associated with this error.
         */
        windowDesc whichWindow() const noexcept { return affectedWindow; }

        /// @brief Whether the error is fatal.
        bool isFatal() const noexcept { return lvl == Level::Fatal; }

        /// @brief Whether the error can be retried.
        bool isRetriable() const noexcept { return lvl == Level::Retry; }

        /// @brief Converts to `true` if this instance represents an actual error.
        explicit operator bool() const noexcept { return lvl != Level::None; }

        #ifdef _WIN32

        /**
         * @brief Builds an error from a raw Windows error code.
         *
         * @details
         * Maps a `DWORD` returned by Windows APIs (from `GetLastError()`)
         * into a structured `DeskUp::Error`, using internal heuristics to
         * determine the proper `Level` and `ErrType`.
         *
         * @param error The Windows error code (e.g., `ERROR_ACCESS_DENIED`).
         * @param context Optional short description of the operation being performed.
         * @param tries Optional number of attempts performed before failure.
         * @return A structured `DeskUp::Error` corresponding to the Windows failure.
         *
         * @see GetLastError
         * @version 0.2.1
         * @date 2025
         */
        static Error fromLastWinError(DWORD error, std::string_view context = "", std::optional<unsigned int> tries = std::nullopt);

        /**
         * @brief Convenience overload that directly fetches the last system error.
         *
         * @param context Optional short description of the operation being performed.
         * @param tries Optional number of attempts performed before failure.
         * @return A structured `DeskUp::Error` corresponding to the last Windows error.
         * @version 0.2.1
         * @date 2025
         */
        static Error fromLastWinError(std::string_view context = "", std::optional<unsigned int> tries = std::nullopt);

        #endif

        /**
         * @brief Converts a `SaveErrorCode` (from `window_desc.cc`) into a structured error.
         *
         * @param e Integer error code returned by a save operation.
         * @return A structured @ref Error describing the save failure.
         * @version 0.2.1
         * @date 2025
         */
        static Error fromSaveError(int e);

    private:
        Level lvl;                /**< Error severity level. */
        ErrType errType;          /**< Error category/type. */
        unsigned int retries;     /**< Number of retries attempted. */
        windowDesc affectedWindow;/**< Optional descriptor of the affected window. */
    };

    /**
     * @typedef Result
     * @brief Alias for an operation result that either holds a value or a DeskUp error.
     *
     * @tparam T The success type.
     * @version 0.2.1
     * @date 2025
     */
    template <typename T>
    using Result = std::expected<T, DeskUp::Error>;

    /**
     * @typedef Status
     * @brief Alias for an operation that returns success or failure (void on success).
     *
     * @version 0.2.1
     * @date 2025
     */
    using Status = std::expected<void, DeskUp::Error>;
}

#endif