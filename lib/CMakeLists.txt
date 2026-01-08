
add_library(omnixlib oxlib.cpp "include/oxlib/math.h")
target_include_directories(omnixlib PUBLIC include)

set_property(TARGET omnixlib PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
