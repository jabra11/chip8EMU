[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/jabra98/chip8EMU.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/jabra98/chip8EMU/context:cpp)

# chip8EMU
The goal of this project is to implement a Chip-8 interpreter. This is a starter project for me to get into low-level programming and emulations. It's using C++17 features so you will need a more or less up-to-date compiler to build it. Build instructions are below. 
First some clips of it being in use in it's current state.

![chip8EMU1](https://user-images.githubusercontent.com/34109209/78613490-4fde6f00-786c-11ea-9dae-7b8a814b7a31.gif)

![chip8EMU2](https://user-images.githubusercontent.com/34109209/78613506-57057d00-786c-11ea-9fba-0ff17b5cf278.gif)

# Dependencies
- Compiler with C++17 support
- [CMake](https://cmake.org/)
- [SFML](https://www.sfml-dev.org/)

# Build Instructions
### Linux
Install the dependencies and then run
```console
$ mkdir build
$ cd build/
$ cmake ..
$ cmake --build .
$ ./Chip8EMU
```

### Windows
Install the dependencies and then run
```console
$ mkdir build
$ cd build/
$ cmake ..
$ cmake --build .
// copy SFML dll's into the build directory
$ ./Chip8EMU
```
