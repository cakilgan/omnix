
#include <omnix/runtime.h>
#include <omnix/platform/crash.h>
#include <omnix/runtime/logger.h>

#include <omnix/runtime/setup.h>

#include "omnix/kernel/s_context.h"

ox::result_t engine::runtime::boot(const config& config, const args& args) {
    setup_context cx{};
    setup(&cx);
    return kernel::context->init();
}

ox::result_t engine::runtime::pump(const ox::time dt) {
    return kernel::context->tick(dt);
}

ox::result_t engine::runtime::shutdown_context() {
    return kernel::context->shutdown();
}