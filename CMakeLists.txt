cmake_minimum_required(VERSION 3.13)

project(chip8EMU)

set(CMAKE_CXX_STANDARD 17)

set(src
    src/main.cpp
    src/Emulator.cpp
    src/Emulator.hpp
    src/Game_selector.cpp
    src/Game_selector.hpp
    src/cpu/CPU.cpp
    src/cpu/CPU.hpp
    src/cpu/Registers.cpp
    src/cpu/Registers.hpp
    src/cpu/Instructions.hpp
    src/cpu/CPU_timer.hpp
    src/display/Display.hpp
    src/display/Display.cpp
    src/keyboard/Keyboard.hpp
    src/keyboard/Keyboard.cpp
    src/memory/Memory.hpp
    src/memory/Memory.cpp
    src/utility/Object_manager.hpp
    src/utility/Object_manager.cpp
    src/utility/Timer.hpp
    )

find_package(SFML COMPONENTS graphics audio window system REQUIRED)

add_executable(chip8EMU ${src})
target_link_libraries(chip8EMU
            PRIVATE
            sfml-graphics
            sfml-audio
            sfml-window
            sfml-system
            )       
            
            
