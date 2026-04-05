add_library(oxlayer_entrypoint STATIC
        src/main.cpp
)

target_include_directories(oxlayer_entrypoint PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/platform/include
        ${CMAKE_SOURCE_DIR}/core/include
        ${CMAKE_SOURCE_DIR}/runtime/include
        ${CMAKE_SOURCE_DIR}/kernel/include
)

target_link_libraries(oxlayer_entrypoint PUBLIC
        omnix_engine
)