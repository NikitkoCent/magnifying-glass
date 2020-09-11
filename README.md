# magnifying-glass
C++17-compatible implementation of optimal algorithm of the magnifying glass effect.

The project consists of three main parts:
* [src/lib/mglass](src/lib/mglass) (+[include/mglass](include/mglass)) - the `mglass` library provides all functionality
  of the magnifying glass effect.
* [src/products/mglass-cli](src/products/mglass-cli) - CLI-application-example of use `mglass` lib.
* [src/products/mglass-gui](src/products/mglass-gui) - GUI-application-example of use `mglass` lib.
  Based on [Qt5](https://www.qt.io/developers).

## Build requirements
* C++17-compatible compiler. Tested with:
  * `MSVC 19.27`;
  * `Apple Clang 11.0.0`;
  * `Apple Clang 11.0.3`;
  * `GCC 8.4.0`.
* [CMake](https://cmake.org/download/) >= `v3.8.2`;
* _Optional_: Qt5 for building `mglass-gui` target.

## Setting up, building and running tests
See [Build requirements](#build-requirements) chapter first.
Also make sure you initialized all [submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) of the project:
```bash
git submodule init
git submodule update
```

Then just use CMake:
```bash
# Build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Running tests
ctest -C Release -V
```

If you want to build `mglass-gui` target too, you should to let know CMake where to search Qt,
e.g. using [CMAKE_PREFIX_PATH](https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html) variable:
```bash
cd build
cmake [-G"<generator>"] -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="<Path/To/Qt>/<version>/<compiler>/lib/cmake" ..
```
See more info in [Qt tutorial](https://doc.qt.io/qt-5/cmake-get-started.html).

## mglass-cli usage
Run `mglass-cli --help` for usage help.
