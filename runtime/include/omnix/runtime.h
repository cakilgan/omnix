#ifndef OMNIX_RUNTIME_H
#define OMNIX_RUNTIME_H

#include <omnix/platform.h>
#include "runtime/args.h"
#include "runtime/config.h"

namespace engine {
    namespace kernel {
        struct context;
    }
    namespace runtime {

        /// Memory + platform ready; runs kernel lifecycle `init` (boot → init hooks).
        ox::result_t boot(kernel::context* context, const config& config, const args& args);

        /// One frame: pre_tick → tick → post_tick.
        ox::result_t pump(kernel::context* context, ox::f32 dt);

        /// Lifecycle shutdown chain (pre_shutdown → shutdown → post_shutdown).
        ox::result_t shutdown_context(kernel::context* context);

        ox::i32 normalize(ox::result_t boot_result);
    }
}

#endif //OMNIX_RUNTIME_H
