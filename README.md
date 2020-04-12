[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/jabra98/chip8EMU.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/jabra98/chip8EMU/context:cpp)

# chip8EMU
The goal of this project is to implement a Chip-8 interpreter. This is a starter project for me to get into low-level programming and emulations. It's using C++17 features so you will need a more or less up-to-date compiler to build it. Build instructions are below. 
First some clips of it being in use in it's current state.

### The game selector (BETA)
![chip8EMU1](https://user-images.githubusercontent.com/34109209/79068709-36548180-7cc9-11ea-8205-3c5eea9a732a.gif)


### Playing WIPEOFF
![chip8EMU2](https://user-images.githubusercontent.com/34109209/79055105-88a28d80-7c4a-11ea-9040-13f0af89f284.gif)

### Changing the clockspeed while playing
![chip8EMU3](https://user-images.githubusercontent.com/34109209/79055106-893b2400-7c4a-11ea-83af-9a4dcdf40f95.gif)

### Stepping through the clockcycles
![chip8EMU4](https://user-images.githubusercontent.com/34109209/79055107-8a6c5100-7c4a-11ea-8b15-e68b2812e26d.gif)

### Red clockspeed means the emulator can't go faster
![chip8EMU5](https://user-images.githubusercontent.com/34109209/79055109-8b9d7e00-7c4a-11ea-8076-5a5dfb93a926.gif)

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
