//
// Created by cakilgan on 3/16/26.
//

#include "omnix/core/allocators/freelist_allocator.h"
#include "omnix/core/engine_memory_internal.h"

namespace engine::memory {
    namespace general {
        ox::freelist_allocator* small  = nullptr;
        ox::freelist_allocator* medium = nullptr;
        ox::freelist_allocator* big  = nullptr;
    }
    namespace kernel {
        ox::freelist_allocator* single = nullptr;
    }
}