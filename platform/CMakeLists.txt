project(omnix_platform)
cmake_minimum_required(VERSION 4.1)

add_library(omnix_platform
        src/platform.cpp
        src/types.cpp
)

if(UNIX)
    target_sources(omnix_platform PRIVATE
            src/linux/file.linux.cpp
            src/linux/time.linux.cpp
            src/linux/memory.linux.cpp
            src/linux/assert.linux.cpp
            src/linux/crash.linux.cpp
            src/linux/date.linux.cpp
    )
    target_link_libraries(omnix_platform PUBLIC xcb)
elseif(WIN32)
    #implement
endif()

target_sources(omnix_platform PRIVATE
        src/file.cpp
)

target_include_directories(omnix_platform PUBLIC include)