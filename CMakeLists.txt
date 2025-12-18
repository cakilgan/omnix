cmake_minimum_required(VERSION 3.31.0)
project(omnix VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(omnix STATIC sources/omnix.cpp sources/oxtime.cpp)
target_include_directories(omnix PUBLIC include)

file(GLOB TESTFILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

add_executable(ox_test
${TESTFILES}
sources/oxtest.cpp)
target_link_libraries(ox_test omnix)


add_executable(omnix_run main.cpp)
target_link_libraries(omnix_run PUBLIC omnix)

