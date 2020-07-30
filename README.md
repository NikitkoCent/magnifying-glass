# magnifying-glass
Implementation of optimal algorithm of the magnifying glass effect.

**TODO: more detailed description of the project.**

## Requirements
**TODO: fill the chapter.**

## Setting up, building and running tests
See [Requirements](#requirements) chapter first.
Also make sure you initialized all [submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) of the project:
```bash
git submodule init
git submodule update
```

Then just use [CMake](https://cmake.org/download/):
```bash
# Build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Running tests
ctest -C Release -V
```
