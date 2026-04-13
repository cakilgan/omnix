//
// Created by cakilgan on 3/30/26.
//

#ifndef OMNIX_ENGINE_MEMORY_INTERNAL_H
#define OMNIX_ENGINE_MEMORY_INTERNAL_H
#include "allocators/freelist_allocator.h"
#include <omnix/platform/types.h>

namespace engine::memory {
    enum type {
        GENERAL,
        KERNEL
    };
    namespace general {
        extern ox::freelist_allocator* small;
        extern ox::freelist_allocator* medium;
        extern ox::freelist_allocator* big;
    }
    namespace kernel{
        extern ox::freelist_allocator* single;
    }

    template<typename T>
    ox::allocator* get_allocator(type memory_type) {
        constexpr auto size = ox::size_of<T>();
        if (memory_type == GENERAL) {
            if constexpr (size < ox::bytes(32))  return general::small;
            if constexpr (size > ox::bytes(256)) return general::big;
            return general::medium;
        }
        if (memory_type == KERNEL){
            return kernel::single;
        }
        return nullptr;
    }



}
#endif //OMNIX_ENGINE_MEMORY_INTERNAL_H
