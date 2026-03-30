add_executable(oxlayer_entrypoint
        src/main.cpp
)

target_compile_definitions(oxlayer_entrypoint PRIVATE
        APP_HEADER_PATH="${APP_HEADER}"
        APP_CLASS_NAME=${APP_CLASS}
)

target_link_libraries(oxlayer_entrypoint PRIVATE
        omnix_engine
)