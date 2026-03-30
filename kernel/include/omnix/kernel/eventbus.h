#ifndef OMNIX_EVENTBUS_H
#define OMNIX_EVENTBUS_H

#include <omnix/platform/types.h>
#include <omnix/core/vector.h>
#include <omnix/core/hashmap.h>

namespace ox {
    struct eventbus {
    private:
        struct handler {
            OX_USING(call_t, void(*)(cvptr));

            call_t trampoline;
            cvptr  original_fn;

            template<typename T, void(*fn)(const T*)>
            static handler make() {
                return handler {
                    [](cvptr p) {
                        fn(static_cast<const T*>(p));
                    },
                    reinterpret_cast<cvptr>(fn)
                };
            }
        };

        hashmap<types::type_id, vector<handler>> _listeners;

    public:
        explicit eventbus(freelist_allocator* allocator)
            : _listeners(allocator) {}

        template<typename T, void(*fn)(const T*)>
        void subscribe() {
            auto& handlers = _listeners.get_or_emplace(
                types::type_of<T>(),
                _listeners.allocator(),
                4
            );
            handlers.push_back(handler::make<T, fn>());
        }

        template<typename T, void(*fn)(const T*)>
        void unsubscribe() {
            auto* handlers = _listeners.get(types::type_of<T>());
            if (!handlers) return;

            cvptr target_addr = reinterpret_cast<cvptr>(fn);

            for (usize i = 0; i < handlers->size(); ++i) {
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
            auto* handlers = _listeners.get(types::type_of<T>());
            if (!handlers) return;

            for (auto& h : *handlers) {
                h.trampoline(static_cast<cvptr>(data));
            }
        }
    };
}

#endif // OMNIX_EVENTBUS_H