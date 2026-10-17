cmake_minimum_required(VERSION 3.16)

project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
        src/Application.cpp
        src/Instruction/instruction.cpp
        src/Strategy/strategy.cpp
)

target_include_directories(app PRIVATE
        src/Instruction
        src/Strategy
)

target_compile_options(app PRIVATE
        -Wall
        -Wextra
)
