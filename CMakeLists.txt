cmake_minimum_required (VERSION 3.31)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project ("omnix")


add_subdirectory(glfw-3.4)

add_subdirectory(lib)

add_subdirectory(modules)

add_subdirectory(examples/checknull)


