cmake_minimum_required(VERSION 4.1)
project(omnix)

# ------------------------------
# Build types
# ------------------------------
set(VALID_BUILD_TYPES Debug Release Product RelDebugInfo)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${VALID_BUILD_TYPES})

# ------------------------------
# Preprocessor defines
# ------------------------------
add_compile_definitions(
        $<$<CONFIG:Debug>:OX_DEBUG>
        $<$<CONFIG:Release>:OX_RELEASE>
        $<$<CONFIG:Product>:OX_PRODUCT>
        $<$<CONFIG:RelDebugInfo>:OX_RELDEBUGINFO>
)

# ------------------------------
# Standard settings
# ------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ------------------------------
# Compiler flags per build type
# ------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Configuring Debug build")
    add_compile_options(-O0 -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Configuring Release build")
    add_compile_options(-O3)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelDebugInfo")
    message(STATUS "Configuring RelDebugInfo build")
    add_compile_options(-O2 -g -fno-omit-frame-pointer)
elseif(CMAKE_BUILD_TYPE STREQUAL "Product")
    message(STATUS "Configuring Product build")
    add_compile_options(-O3)
endif()

# ------------------------------
# Source files
# ------------------------------
file(GLOB_RECURSE OX_SOURCES "src/*.cpp")

# ------------------------------
# Subdirectories
# ------------------------------
add_subdirectory(platform)
add_subdirectory(core)
add_subdirectory(kernel)
add_subdirectory(runtime)

# ------------------------------
# Engine library
# ------------------------------
add_library(omnix_engine ${OX_SOURCES})
target_include_directories(omnix_engine PUBLIC "include")
target_link_libraries(omnix_engine PRIVATE oxlayer_platform oxlayer_core oxlayer_kernel oxlayer_runtime)

# ------------------------------
# Executable
# ------------------------------
add_subdirectory(entrypoint)


add_subdirectory(game)