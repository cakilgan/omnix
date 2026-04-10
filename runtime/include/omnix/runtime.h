#ifndef OMNIX_RUNTIME_H
#define OMNIX_RUNTIME_H

#include <omnix/platform.h>
#include "runtime/args.h"
#include "runtime/config.h"

namespace engine {
    namespace runtime {
        extern enum class states {
            not_started,
            init,
            run,
            shutdown,
            stop
        }state;

        ox::result_t boot(const config& config, const args& args);

        ox::result_t pump(ox::time dt);

        ox::result_t shutdown_context();

    }
}

#endif //OMNIX_RUNTIME_H
