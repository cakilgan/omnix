
#include <omnix/runtime.h>
#include <omnix/platform/crash.h>

#include <omnix/runtime/setup.h>

#include "omnix/kernel/context.h"

ox::result_t engine::runtime::boot(kernel::context* const context, const config& config, const args& args) {
    setup_context cx{};
    setup(&cx);
    return context->init();
}

ox::result_t engine::runtime::pump(kernel::context* const context, const ox::f32 dt) {
    if (context == nullptr)
        return ox::results::err::null_pointer;
    return context->tick(dt);
}

ox::result_t engine::runtime::shutdown_context(kernel::context* const context) {
    if (context == nullptr)
        return ox::results::err::null_pointer;
    return context->shutdown();
}

ox::i32 engine::runtime::normalize(ox::result_t boot_result) {
    if (boot_result != ox::ok) {
        OX_CRASH("boot returned something unusal!");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
