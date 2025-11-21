/**
 * @file desk_up_dummy_device.h
 * @brief Dummy test device for backend interface testing
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
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DESK_UP_DUMMY_DEVICE_H
#define DESK_UP_DUMMY_DEVICE_H

#include <vector>
#include <string>
#include <filesystem>

#include "desk_up_window_device.h"
#include "window_desc.h"
#include "desk_up_error.h"

struct DummyDeviceData {
    int x = 100;
    int y = 150;
    unsigned int w = 800;
    unsigned int h = 600;
    std::string path = "/dummy/test.exe";
    std::vector<windowDesc> windows;
	bool forceNonEmpty = true;
    bool simulateError = false;
    DeskUp::Error errorToReturn = {DeskUp::Level::Fatal, DeskUp::ErrType::Default, 0, "Dummy error"};
};

// Stub function implementations
inline DeskUp::Result<unsigned int> DUMMY_getWindowHeight(DeskUpWindowDevice* _this) {
    auto* data = static_cast<DummyDeviceData*>(_this->internalData);
    if (data->simulateError) return std::unexpected(data->errorToReturn);
    return data->h;
}

inline DeskUp::Result<unsigned int> DUMMY_getWindowWidth(DeskUpWindowDevice* _this) {
    auto* data = static_cast<DummyDeviceData*>(_this->internalData);
    if (data->simulateError) return std::unexpected(data->errorToReturn);
    return data->w;
}

inline DeskUp::Result<int> DUMMY_getWindowXPos(DeskUpWindowDevice* _this) {
    auto* data = static_cast<DummyDeviceData*>(_this->internalData);
    if (data->simulateError) return std::unexpected(data->errorToReturn);
    return data->x;
}

inline DeskUp::Result<int> DUMMY_getWindowYPos(DeskUpWindowDevice* _this) {
    auto* data = static_cast<DummyDeviceData*>(_this->internalData);
    if (data->simulateError) return std::unexpected(data->errorToReturn);
    return data->y;
}

inline DeskUp::Result<fs::path> DUMMY_getPathFromWindow(DeskUpWindowDevice* _this) {
    auto* data = static_cast<DummyDeviceData*>(_this->internalData);
    if (data->simulateError) return std::unexpected(data->errorToReturn);
    return fs::path(data->path);
}

inline DeskUp::Result<std::string> DUMMY_getDeskUpPath() {
    // Return a test-specific path

	fs::path ret = std::filesystem::temp_directory_path();

	ret /= "DeskUpTest";

    return ret.string();
}

inline DeskUp::Result<std::vector<windowDesc>> DUMMY_getAllOpenWindows(DeskUpWindowDevice* _this) {
    auto* data = static_cast<DummyDeviceData*>(_this->internalData);
    if (data->simulateError) return std::unexpected(data->errorToReturn);
    // Populate once if empty; otherwise return cached
    if (data->windows.empty() && data->forceNonEmpty) {
        for (int i = 0; i < 4; ++i) {
            std::string name = std::string("testWindow_") + std::to_string(i);
            std::string path = name + ".exe";
            windowDesc w{name, data->x + i * 10, data->y + i * 5,
                         static_cast<unsigned int>(data->w + i * 20),
                         static_cast<unsigned int>(data->h + i * 15),
                         path};
            data->windows.push_back(std::move(w));
        }
    }
    return data->windows;
}

inline DeskUp::Status DUMMY_loadWindowFromPath(DeskUpWindowDevice* _this, const fs::path& path) {
    auto* data = static_cast<DummyDeviceData*>(_this->internalData);
    if (data->simulateError) return std::unexpected(data->errorToReturn);
    if (path.empty()) {
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0, "Empty path"
        ));
    }
    data->path = path.string();
    return {};
}

inline DeskUp::Result<windowDesc> DUMMY_recoverSavedWindow(DeskUpWindowDevice* _this, const fs::path& filePath) {
    auto* data = static_cast<DummyDeviceData*>(_this->internalData);
    if (data->simulateError) return std::unexpected(data->errorToReturn);

    if (!std::filesystem::exists(filePath)) {
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0, "File not found"
        ));
    }

    // Return a dummy window
    return windowDesc{"DummyApp", data->x, data->y, data->w, data->h, data->path};
}

inline DeskUp::Status DUMMY_resizeWindow(DeskUpWindowDevice* _this, const windowDesc window) {
    auto* data = static_cast<DummyDeviceData*>(_this->internalData);
    if (data->simulateError) return std::unexpected(data->errorToReturn);

    // Update internal state
    data->x = window.x;
    data->y = window.y;
    data->w = window.w;
    data->h = window.h;
    return {};
}

inline DeskUp::Result<unsigned int> DUMMY_closeProcessFromPath(DeskUpWindowDevice* _this, const fs::path& path, bool allowForce) {
    auto* data = static_cast<DummyDeviceData*>(_this->internalData);
    if (data->simulateError) return std::unexpected(data->errorToReturn);

    if (path.empty()) {
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0, "Empty path"
        ));
    }

    // Simulate closing processes (return a dummy count)
    return 1u;
}

/**
 * @brief Creates a dummy device for testing
 */
inline DeskUpWindowDevice DUMMY_CreateDevice() {
    DeskUpWindowDevice device;

    device.getWindowHeight = DUMMY_getWindowHeight;
    device.getWindowWidth = DUMMY_getWindowWidth;
    device.getWindowXPos = DUMMY_getWindowXPos;
    device.getWindowYPos = DUMMY_getWindowYPos;
    device.getPathFromWindow = DUMMY_getPathFromWindow;
    device.getDeskUpPath = DUMMY_getDeskUpPath;
    device.getAllOpenWindows = DUMMY_getAllOpenWindows;
    device.loadWindowFromPath = DUMMY_loadWindowFromPath;
    device.recoverSavedWindow = DUMMY_recoverSavedWindow;
    device.resizeWindow = DUMMY_resizeWindow;
    device.closeProcessFromPath = DUMMY_closeProcessFromPath;

    device.internalData = new DummyDeviceData();

    return device;
}

/**
 * @brief Helper to get typed access to dummy device data
 */
inline DummyDeviceData* DUMMY_GetData(DeskUpWindowDevice* device) {
    return static_cast<DummyDeviceData*>(device->internalData);
}

/**
 * @brief Helper to clean up dummy device
 */
inline void DUMMY_DestroyDevice(DeskUpWindowDevice* device) {
    if (device && device->internalData) {
        delete static_cast<DummyDeviceData*>(device->internalData);
        device->internalData = nullptr;
    }
}

#endif // DESK_UP_DUMMY_DEVICE_H
