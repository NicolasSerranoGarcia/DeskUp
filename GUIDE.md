> [!TIP]
> If you are planning to get involved with the source code, here is a list of utils and instructions you can take a look at


# 🛠️ Project Build & Workflow Guide

> **📍 Note:** All commands are intended to be executed from within the `build/` directory.

---

## Required installs

Using MSYS2 MINGW64:

```bash
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-qt6
pacman -S mingw-w64-x86_64-qt6-base
pacman -S mingw-w64-x86_64-qt6-tools
pacman -S --needed mingw-w64-x86_64-graphviz
pacman -Syu
pacman -S mingw-w64-x86_64-texlive-bin \
          mingw-w64-x86_64-texlive-core \
          mingw-w64-x86_64-texlive-latex-recommended \
          mingw-w64-x86_64-texlive-latex-extra
pacman -S mingw-w64-x86_64-texlive-plain-generic
pacman -S mingw-w64-x86_64-texlive-fonts-recommended
pacman -S mingw-w64-x86_64-texlive-extra-utils \
          mingw-w64-x86_64-ghostscript
```

## 📁 Initial Setup

1. If `build/` doesn't exist, create it:

   ```bash
   mkdir build
   ```

2. Navigate to the `build/` directory:

   ```bash
   cd build
   ```

3. Run CMake to generate the build system:

   ```bash
   cmake ../
   ```

   Optionally, you can add flags to enable tools or configure the build (see below).

4. Build the project:

   ```bash
   cmake --build .
   ```

5. Run the program manually to check if the added code works.

6. Run the test suite:

   ```bash
   ctest
   ```

   - To run specific tests by name:

     ```bash
     ctest -R <regex>
     ```

     Example:
     ```bash
     ctest -R class
     ```

     > **Note:** Google Test uses the format `<test_suite>.<test_name>`, so you can target individual tests like:
     ```bash
     ctest -R MyTestSuite.MyTestCase
     ```

7. Write new tests for your feature.

8. Commit your changes to the current branch (see commit format below).

9. If you have a CI pipeline, it should run the tests across different OSes. If not, test manually.

10. If something breaks, return to step 3, fix the problem, and do **not** push until resolved.

11. Once the feature is complete and stable, consider creating a release.

---

## ⚙️ CMake Build Flags

You can pass these flags when running `cmake ..` to control what gets built or enabled.

### Build Type

You can build the program in different types, depending on if you want a debug or a release build. These flags are provided by CMake itself, and run optimizations, debug flags... that are useful for each use case

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Note that it doesn't make sense to use the upcoming flags for a release type.


### 🔬 Build Options

| Flag                     | DESCRIPTION                       | Default |
|--------------------------|--------------------------------------|---------|
| `-DBUILD_TESTS=ON\|OFF`   | Enables/disables building tests      | OFF    |
| `-DBUILD_BENCHMARK=ON\|OFF`| Enables/disables building benchmarks | OFF      |

### 🧼 Sanitizers

Enable runtime checks for common bugs:

```bash
cmake .. -DUSE_SANITIZER=address
cmake .. -DUSE_SANITIZER=thread
cmake .. -DUSE_SANITIZER=memory #Not available 
cmake .. -DUSE_SANITIZER=memorywithorigins #Not available
cmake .. -DUSE_SANITIZER=undefined
cmake .. -DUSE_SANITIZER=leak
cmake .. -DUSE_SANITIZER="address;undefined"  # multiple sanitizers
```


### 🔍 Static Analyzers

Run static analysis tools during the build:


Note that this tools are only available for Linux. Also note that you need to install the tools you want to use. 

To install all three run 

```bash
sudo apt install clang-tidy
sudo apt install cppcheck
sudo apt install iwyu
```
on your terminal.

```bash
cmake .. -DUSE_STATIC_ANALYZER=clang-tidy
cmake .. -DUSE_STATIC_ANALYZER=cppcheck
cmake .. -DUSE_STATIC_ANALYZER=iwyu
cmake .. -DUSE_STATIC_ANALYZER="clang-tidy;cppcheck"  # multiple tools
```

> Note: this tools will scan ALL your project, including third-party dependencies. This means it will scan libraries which will be superfluous in most cases, so keep in mind when using it

---

## 📦 Other CMake Tools

### 🏗️ Build and Install

```bash
cmake --build .
cmake --install .
```

Installs the project to the `install/` directory.

### 📦 CPack (Packaging)

Run to create platform-specific install packages:

```bash
cpack
```

---

### 📦 Generate AppImage

| Flag                     | DESCRIPTION                       | Default |
|--------------------------|--------------------------------------|---------|
| `-DGENERATE_APPIMAGE=ON\|OFF` | Enables/disables the creation of an appImage | OFF |

> Note: enabling `-DGENERATE_APPIMAGE` will have no effect on windows

### Generate the DeskUp installer

When developing, qt get's linked dynamically, so no there is no dependency problem. When shipping the executable, you must ship the all the qt environment too. For this, DeskUp uses an installer created with Inno Setup. 

If you know how to work with Inno Setup, the `.iss` is inside `installation/`.

If you don't, you first need to install [Inno Setup](https://jrsoftware.org/isinfo.php), then go into `build/` and run:

```bash
windeployqt6 DeskUp.exe --release --dir ../build_inno
cd ..
cd installation
echo 'export PATH="$PATH:/c/Program Files (x86)/Inno Setup 6"' >> ~/.bashrc
source ~/.bashrc
ISCC.exe "DeskUp-installer.iss"
```

The `.exe` installer will be inside `build_inno/Output`

## 📚 Documentation with Doxygen

### 📝 Generate Documentation

| Flag                     | DESCRIPTION                       | Default |
|--------------------------|--------------------------------------|---------|
| `-DGNERATE_DOCS=ON\|OFF` | Enables/disables the creation of docs | OFF |


on /build:

```bash
doxygen -u Doxyfile
doxygen Doxyfile
```
on docs/latex:

```bash
pdflatex --shell-escape refman.tex
```

### 🧾 Example of Doxygen-style Comment:

```cpp
/**
 * @brief Adds two integers.
 * 
 * @details This function takes two integers as input and returns their sum.
 * 
 * @param a First integer.
 * @param b Second integer.
 * @return The result of a + b.
 * 
 * @author your name <your gmail>
 * @date 2025
 * @version x.y
 */
int add(int a, int b);
```

---

## 📌 Commit Message Convention (Conventional Commits)

### ✍️ Format

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

- `type` can be:
  - `feat` – New feature
  - `fix` – Bug fix
  - `docs` – Documentation change
  - `style` – Code formatting (no logic change)
  - `refactor` – Code refactor (no behavior change)
  - `perf` – Performance improvements
  - `test` – Adding/modifying tests
  - `build` – Build system or dependency changes
  - `ci` – Continuous Integration changes
  - `chore` – Other changes (e.g., maintenance)

> Note that you should only use one type per commit. Additional info can go in parenthesis before the ":"

---

### 💻 Commit from Git Bash

```bash
git commit -m "feat(parser): add support for config files" \
            -m "Adds YAML and JSON config parsing modules.\n\nIncludes unit tests and default config loading." \
            -m "Closes #42"
```
