#ifndef OMNIX_EVENTBUS_H
#define OMNIX_EVENTBUS_H

#include "omnix/core/engine_memory_internal.h"
#include <omnix/platform/types.h>
#include <omnix/core/vector.h>
#include <omnix/core/hashmap.h>

namespace engine {
    struct eventbus {
    private:
        struct handler {
            OX_USING(call_t, void(*)(ox::cvptr));

            call_t trampoline;
            ox::cvptr  original_fn;

            template<typename T, void(*fn)(const T*)>
            static handler make() {
                return handler {
                    [](ox::cvptr p) {
                        fn(static_cast<const T*>(p));
                    },
                    reinterpret_cast<ox::cvptr>(fn)
                };
            }
        };

        ox::hashmap<ox::types::type_id, ox::vector<handler>> _listeners;

    public:
        explicit eventbus()
            : _listeners(engine::memory::get_allocator<bool>(engine::memory::KERNEL)) {}

        template<typename T, void(*fn)(const T*)>
        void subscribe() {
            auto& handlers = _listeners.get_or_emplace(
                ox::types::type_of<T>(),
                _listeners.allocator(),
                4
            );
            handlers.push_back(handler::make<T, fn>());
        }

        template<typename T, void(*fn)(const T*)>
        void unsubscribe() {
            auto* handlers = _listeners.get(ox::types::type_of<T>());
            if (!handlers) return;

            auto target_addr = reinterpret_cast<ox::cvptr>(fn);

            for (ox::usize i = 0; i < handlers->size(); ++i) {
                if ((*handlers)[i].original_fn == target_addr) {
                    if (i < handlers->size() - 1) {
                        (*handlers)[i] = handlers->back();
                    }
                    handlers->pop_back();
                    return;
                }
            }
        }

        template<typename T>
        void emit(const T* data) {
            auto* handlers = _listeners.get(ox::types::type_of<T>());
            if (!handlers) return;

            for (auto& h : *handlers) {
                h.trampoline(static_cast<ox::cvptr>(data));
            }
        }
    };
}

#endif // OMNIX_EVENTBUS_H