cmake_minimum_required (VERSION 3.31)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project ("omnix")


add_subdirectory(glfw-3.4)

add_subdirectory(lib)

add_subdirectory(modules)

add_subdirectory(checknull)

add_executable (omnix "main.cpp"    "omnix/omnix.h" "omnix/omnix.cpp" "omnix/common.h" "omnix/common.cpp" "omnix/zeus.h" "glad/src/gl.c" "omnix/zeus.cpp" "omnix/stb_image.h")
target_include_directories(omnix PUBLIC core glad/include)
target_link_libraries(omnix PUBLIC omnixlib window_mod input_mod checknull)

set_property(TARGET omnix PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

