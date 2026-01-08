add_library(checknull "checknull_Module.h" "checknull_Module.cpp")
target_link_libraries(checknull PUBLIC omnixlib window_mod input_mod ui_mod)
target_include_directories(checknull PUBLIC ../omnix ../glad/include)
set_property(TARGET checknull PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_executable (checknull_run "main.cpp" 
"../../omnix/omnix.h" 
"../../omnix/omnix.cpp" 
"../../omnix/common.h" 
"../../omnix/common.cpp" 
"../../omnix/zeus.h" 
"../../glad/src/gl.c" 
"../../omnix/zeus.cpp" 
"../../omnix/stb_image.h")
target_include_directories(checknull_run PUBLIC "../" core glad/include)
target_link_libraries(checknull_run PUBLIC omnixlib window_mod input_mod checknull)
set_property(TARGET checknull_run PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)