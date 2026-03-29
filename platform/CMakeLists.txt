add_library(oxlayer_platform
        src/platform.cpp
        src/types.cpp
)

if(UNIX)
    target_sources(oxlayer_platform PRIVATE
            src/linux/file.linux.cpp
            src/linux/time.linux.cpp
            src/linux/memory.linux.cpp
            src/linux/assert.linux.cpp
            src/linux/crash.linux.cpp
    )
elseif(WIN32)
    #implement
endif()

target_sources(oxlayer_platform PRIVATE
        src/file.cpp
)

target_include_directories(oxlayer_platform PUBLIC include)