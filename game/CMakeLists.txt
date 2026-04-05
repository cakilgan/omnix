cmake_minimum_required(VERSION 3.20)
project(my_game)

add_executable(my_game setup.cpp)
target_link_libraries(my_game PRIVATE oxlayer_entrypoint)