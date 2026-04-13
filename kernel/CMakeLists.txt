add_library(oxlayer_kernel
        src/kernel.cpp
)

if(UNIX)
    target_sources(oxlayer_kernel PRIVATE
            src/linux/linux_dummy.cpp
    )
elseif(WIN32)
    #implement
endif()

target_include_directories(oxlayer_kernel PUBLIC include)
target_link_libraries(oxlayer_kernel PRIVATE oxlayer_platform oxlayer_core)