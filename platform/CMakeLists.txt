add_library(oxlayer_platform
        src/platform.cpp
        src/types.cpp
        src/assert.cpp
)

if(UNIX)
    target_sources(oxlayer_platform PRIVATE
            src/linux/file.linux.cpp
            src/linux/time.linux.cpp
            src/linux/memory.linux.cpp
    )
elseif(WIN32)
    target_sources(oxlayer_platform PRIVATE
            src/windows/file.windows.cpp
            src/windows/time.windows.cpp
    )
endif()

target_sources(oxlayer_platform PRIVATE
        src/file.cpp
)

target_include_directories(oxlayer_platform PUBLIC include)