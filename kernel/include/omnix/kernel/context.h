//
// Created by cakilgan on 3/30/26.
//

#ifndef OMNIX_CONTEXT_H
#define OMNIX_CONTEXT_H
#include "eventbus.h"

namespace engine {
    namespace kernel {
        struct context {
        private:
            eventbus _bus;
        public:
            context() = default;
            ox::result_t init();
            ox::result_t tick(ox::f32 dt);
            ox::result_t shutdown();

            eventbus* bus() {
                return &_bus;
            }
        };
    }
}
#endif //OMNIX_CONTEXT_H