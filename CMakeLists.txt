cmake_minimum_required(VERSION 4.1)
project(omnix)

set(CMAKE_CONFIGURATION_TYPES Debug Release Product CACHE STRING "" FORCE)
set(VALID_BUILD_TYPES Debug Release Product)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${VALID_BUILD_TYPES})

add_compile_definitions(
        $<$<CONFIG:Debug>:OX_DEBUG>
        $<$<CONFIG:Release>:OX_RELEASE>
        $<$<CONFIG:Product>:OX_PRODUCT>
)

set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE OX_SOURCES "src/*.cpp")

add_subdirectory(platform)

add_library(omnix_engine ${OX_SOURCES})
target_include_directories(omnix_engine PRIVATE "include")
target_link_libraries(omnix_engine PUBLIC oxlayer_platform)

add_executable(omnix main.cpp)
target_include_directories(omnix PRIVATE "include")
target_link_libraries(omnix PUBLIC omnix_engine)
