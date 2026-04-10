//
// Created by cakilgan on 3/30/26.
//

#ifndef OMNIX_CONTEXT_H
#define OMNIX_CONTEXT_H
#include "eventbus.h"

namespace engine {
    namespace kernel {
        struct s_context {
        private:
            eventbus _bus;
        public:
            s_context() = default;

            ox::result_t init();
            ox::result_t tick(ox::time dt);
            ox::result_t shutdown();

            eventbus* bus() {
                return &_bus;
            }
        };

        extern s_context* context;
    }
}
#endif //OMNIX_CONTEXT_H