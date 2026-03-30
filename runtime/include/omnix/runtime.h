#ifndef OMNIX_RUNTIME_H
#define OMNIX_RUNTIME_H

#include <omnix/platform.h>
#include "runtime/args.h"
#include "runtime/config.h"

namespace engine {
    namespace runtime{
        ox::result_t boot(const engine::config& config,const engine::args& args);
        ox::i32 normalize(ox::result_t boot_result);
    }
}






#endif //OMNIX_RUNTIME_H
