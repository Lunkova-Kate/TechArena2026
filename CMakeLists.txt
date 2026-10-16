cmake_minimum_required(VERSION 3.16)

project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/**/*.cpp")

add_executable(app ${SOURCES})

target_include_directories(app PRIVATE src src/strategy src/instruction)

target_compile_options(app PRIVATE
        -Wall
        -Wextra
)