//
// Created by cakilgan on 3/30/26.
//

#ifndef OMNIX_CONTEXT_H
#define OMNIX_CONTEXT_H
#include "eventbus.h"

namespace ox {
    namespace kernel {
        struct context {
        private:
            eventbus _bus;
        public:
            explicit context(freelist_allocator* allocator):_bus(allocator){}
            result_t init();
            result_t tick(f32 dt);
            result_t shutdown();

            eventbus* bus() {
                return &_bus;
            }
        };
    }
}
#endif //OMNIX_CONTEXT_H