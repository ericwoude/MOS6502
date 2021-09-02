

# C++ template

A basic C++ project template with CMake and GoogleTest.

## Usage

1. Configure the project and generate the native build system:
```bash
$ cmake -B build
```

2. Call the build system to compile and link the project
```bash
$ cmake --build build
```
The compiled executables will be placed in build/bin.

3. Test the project
```bash
$ cd build && ctest 
```
