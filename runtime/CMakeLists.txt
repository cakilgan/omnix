add_library(oxlayer_runtime
        src/runtime.cpp
)

target_include_directories(oxlayer_runtime PUBLIC include)
target_link_libraries(oxlayer_runtime PRIVATE omnix_platform oxlayer_core oxlayer_kernel)