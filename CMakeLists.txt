cmake_minimum_required(VERSION 3.14)

project(chip8EMU)

set(CMAKE_CXX_STANDARD_REQUIRED 17)

set(src
    src/main.cpp)

find_package(SFML COMPONENTS graphics audio window system)

add_executable(chip8EMU ${src})
target_compile_options(chip8EMU PRIVATE -Wall -Wextra)
target_link_libraries(chip8EMU
            PRIVATE
            sfml-graphics
            sfml-audio
            sfml-window
            sfml-system)       