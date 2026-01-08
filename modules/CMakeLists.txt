


set(WINDOW_BACKEND "GLFW" CACHE STRING "Window backend")
set_property(CACHE WINDOW_BACKEND PROPERTY STRINGS GLFW NATIVE SDL)

set(WINDOW_BACKEND "GLFW" CACHE STRING "" FORCE)

add_library(window_mod STATIC)
target_link_libraries(window_mod PUBLIC omnixlib)
target_include_directories(window_mod PUBLIC .)

if(WINDOW_BACKEND STREQUAL "GLFW")
    target_sources(window_mod PRIVATE window_module_glfw.cpp)
    target_link_libraries(window_mod PUBLIC glfw)
    target_compile_definitions(window_mod PUBLIC WINDOW_BACKEND_GLFW)

elseif(WINDOW_BACKEND STREQUAL "NATIVE")
    target_sources(window_mod PRIVATE window_module_native.cpp)
    target_compile_definitions(window_mod PUBLIC WINDOW_BACKEND_NATIVE)

elseif(WINDOW_BACKEND STREQUAL "SDL")
    target_sources(window_mod PRIVATE window_module_sdl.cpp)
    target_link_libraries(window_mod PUBLIC SDL2)
    target_compile_definitions(window_mod PUBLIC WINDOW_BACKEND_SDL)

else()
    message(FATAL_ERROR "Unknown WINDOW_BACKEND: ${WINDOW_BACKEND}")
endif()


add_library(input_mod "input_module.h" "input_module.cpp")
target_link_libraries(input_mod PUBLIC omnixlib window_mod)
target_include_directories(input_mod PUBLIC ../omnix)
set_property(TARGET input_mod PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(graphics_mod "graphics_module.h" "graphics_module.cpp")
target_link_libraries(graphics_mod PUBLIC omnixlib window_mod)
target_include_directories(graphics_mod PUBLIC ../omnix ../glad/include)
set_property(TARGET graphics_mod PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ui_mod "ui_module.h" "ui_module.cpp")
target_link_libraries(ui_mod PUBLIC omnixlib window_mod input_mod graphics_mod)
target_include_directories(ui_mod PUBLIC ../omnix ../glad/include)
set_property(TARGET ui_mod PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)