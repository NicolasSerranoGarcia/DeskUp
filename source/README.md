# DeskUp internal functioning

DeskUp is organized into three main layers:

- **Frontend**: GUI of window and workspace operations.
- **Core**: backend initialization and management logic.
- **Backend**: platform-specific implementations for Windows 

This document is here for anyone to have a clear understanding of how DeskUp operates, or at least to know **where** to look for a specific thing

---

## 1️⃣ Initialization — Choosing and bootstrapping a backend

When DeskUp starts, the application calls `DU_Init`
(declared in [`source/desk_up_window_backend/window_core.h`](./desk_up_window_backend/window_core.h)
and implemented in [`source/desk_up_window_backend/window_core.cc`](./desk_up_window_backend/window_core.cc)).

`DU_Init()` performs three key actions:

1. Iterates through a list of available **backend bootstraps** (currently only Windows).
   - The Windows backend defines its bootstrap (`winWindowDevice`) in
     [`source/desk_up_window_backend/window_backends/desk_up_win/desk_up_win.h`](./desk_up_window_backend/window_backends/desk_up_win/desk_up_win.h).

2. For each backend, it checks availability via `DeskUpWindowBootStrap::isAvailable()` (Windows maps to `WIN_isAvailable`).

3. Once it finds a working backend, it:
   - Creates the device with `DeskUpWindowBootStrap::createDevice()` (Windows maps to `WIN_CreateDevice`).
   - Retrieves the base folder with `DeskUpWindowDevice::getDeskUpPath()` (Windows maps to `WIN_getDeskUpPath`).
   - Sets the global variables:
     - **DESKUPDIR** — base workspace directory.
     - **current_window_backend** — active backend device.

If initialization succeeds, `DU_Init()` logs the connected backend and returns `1`.
If none is available, it returns `0`.

---

## 2️⃣ High-level operations — DeskUpWindow façade

`DeskUpWindow` is defined in
[`source/desk_up_window/desk_up_window.h`](./desk_up_window/desk_up_window.h)
and implemented in
[`source/desk_up_window/desk_up_window.cc`](./desk_up_window/desk_up_window.cc).

This class acts as the **frontend façade**, coordinating workspace-level operations with the backend.

### 🔹 `DeskUpWindow::saveAllWindowsLocal(std::string workspaceName)`
1. Builds `<DESKUPDIR>/<workspaceName>` using the global path set by `DU_Init()`.
2. Ensures the directory exists (via `std::filesystem`).
3. Requests the active backend to enumerate all windows through
   `current_window_backend->getAllWindows(current_window_backend.get())`.
4. Receives a list of `windowDesc` records from the backend.
5. Saves each record to a text file using `windowDesc::saveTo()`.

If any backend or I/O operation fails, the function catches the exception and returns `0`, avoiding crashes.

---

## 3️⃣ Window representation — The `windowDesc` structure

Defined in [`source/desk_up_window_backend/window_desc/window_desc.h`](./desk_up_window_backend/window_desc/window_desc.h)
and implemented in [`source/desk_up_window_backend/window_desc/window_desc.cc`](./desk_up_window_backend/window_desc/window_desc.cc).

Each `windowDesc` instance represents a window in an abstract, cross-platform way.

**Fields:**
- `name` — window or executable name.
- `x`, `y`, `w`, `h` — position and size.
- `pathToExec` — absolute path to the owning executable.

**Behavior:**
- `saveTo(path)` writes the above fields as plain text.
- Returns `0` if the path is empty or cannot be opened.
- `operator!()` is a validity check (true if geometry is all zero).

---

## 4️⃣ Backend implementation — Windows

The Windows backend lives in
[`source/desk_up_window_backend/window_backends/desk_up_win/desk_up_win.h`](./desk_up_window_backend/window_backends/desk_up_win/desk_up_win.h)
and [`source/desk_up_window_backend/window_backends/desk_up_win/desk_up_win.cc`](./desk_up_window_backend/window_backends/desk_up_win/desk_up_win.cc).

It defines:
```cpp
DeskUpWindowBootStrap winWindowDevice = {
    "win",
    WIN_CreateDevice,
    WIN_isAvailable
};
```

### 🔹 Backend creation
- `WIN_CreateDevice()` builds a new `DeskUpWindowDevice` and wires function pointers for:
  - `getAllWindows`
  - `getWindowHeight`, `getWindowWidth`, `getWindowXPos`, `getWindowYPos`
  - `getDeskUpPath`
  - ...
  
All the common functions necessary for any platform to make deskUp work. If there are platfrom-specific calls, they won't appear in the API.

### 🔹 Enumerating windows
`WIN_getAllWindows()` calls `EnumDesktopWindows`, which triggers a callback to:
1. Skip invisible or zero-sized windows.
2. Populate a `windowDesc` using:
   - `WIN_getWindowXPos`, `WIN_getWindowYPos`, `WIN_getWindowWidth`, `WIN_getWindowHeight` → `GetWindowInfo`.
   - `WIN_getPathFromWindow` → process path via
     `GetWindowThreadProcessId` → `OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION)` → `QueryFullProcessImageNameW`.
3. Append each valid record to a `std::vector<windowDesc>`.

### 🔹 Workspace path resolution
`WIN_getDeskUpPath()` determines the folder:
- Uses `SHGetKnownFolderPath(FOLDERID_RoamingAppData)` when available.
- Falls back to `%APPDATA%` or the executable directory.
- Ensures a `DeskUp/` folder exists.

---

## 5️⃣ How everything connects — Flow summary

```text
DU_Init()
  ├─> winWindowDevice.isAvailable() → WIN_isAvailable()
  ├─> winWindowDevice.createDevice() → WIN_CreateDevice()
  ├─> dev.getDeskUpPath() → WIN_getDeskUpPath()
  └─> sets DESKUPDIR and current_window_backend

DeskUpWindow::saveAllWindowsLocal("WorkspaceName")
  ├─> builds <DESKUPDIR>\WorkspaceName
  ├─> current_window_backend->getAllWindows(...) → WIN_getAllWindows()
  │     ├─> EnumDesktopWindows → WIN_createAndSaveWindow()
  │     └─> fills std::vector<windowDesc>
  ├─> iterates vector
  │     └─> windowDesc::saveTo(<workspace path>)
  └─> returns success (1) or failure (0)
```

---

## 6️⃣ File map

| Layer | Path | Description |
|-------|------|--------------|
| **Frontend** | `source/desk_up_window/desk_up_window.h` / `.cc` | Orchestrates workspace operations (uses backend). |
| **Core** | `source/desk_up_window_backend/window_core.h` / `.cc` | Backend initialization (`DU_Init`) and global state. |
| **Backend (Windows)** | `source/desk_up_window_backend/window_backends/desk_up_win/desk_up_win.h` / `.cc` | Implements Windows-specific logic. |
| **Window record** | `source/desk_up_window_backend/window_desc/window_desc.h` / `.cc` | Data structure representing windows. |
| **Backend utilities** | `source/desk_up_window_backend/backend_utils.cc` | Shared helper functions for backends. |
| **Interfaces** | `source/desk_up_window_backend/desk_up_window_device.h`, `desk_up_window_bootstrap.h` | Device and bootstrap definitions. |
| **Other components** | `source/desk_up/`, `source/desk_up_frame/` | Higher-level modules (e.g., UI, frame handling). |
| **Entry point** | `source/main.cc` | Program start. |

---

## 7️⃣ Extensibility

DeskUp’s modular design allows adding new platforms easily.

Adding a new backend only requires implementing a `DeskUpWindowBootStrap` with:
- `isAvailable()` → to detect platform support.
- `createDevice()` → to provide the correct function pointers.

It also facilitates creating tests for the backend, as one can create a testing device, which includes predefined values for each function. Multiple devices can be connected, so as to test multiple functionalities.