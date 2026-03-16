add_library(oxlayer_core
        src/core.cpp
)

if(UNIX)
    target_sources(oxlayer_core PRIVATE
            src/linux/linux_dummy.cpp
    )
elseif(WIN32)
    #implement
endif()

target_include_directories(oxlayer_core PUBLIC include)
target_link_libraries(oxlayer_core PRIVATE oxlayer_platform)