//
// Created by cakilgan on 3/7/26.
//

#ifndef OMNIX_MEMORY_H
#define OMNIX_MEMORY_H

#include "defines.h"
#include "types.h"
#include "result.h"
#include "byte.h"

namespace ox {
    //NOTE: not using ::ox::bytes cause of simplicity.

    OX_FORCE_CONSTEXPR usize align(usize size, usize alignment) {
        if (alignment == 0) return size;
        usize remainder = size % alignment;
        if (remainder == 0) return size;
        return size + (alignment - remainder);
    }

    OX_FORCE_CONSTEXPR usize nextpow2(usize x) {
        if (x == 0) return 1;
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        if constexpr (sizeof(usize) > 4) x |= x >> 32;
        return x + 1;
    }

    OX_FORCE_CONSTEXPR bool ispow2(usize x) {
        return x != 0 && (x & (x - 1)) == 0;
    }

    OX_INLINE void* align_ptr(void* OX_RESTRICT ptr, const usize alignment) {
        auto p = reinterpret_cast<uptr>(ptr);
        p = (p + alignment - 1) & ~(alignment - 1);
        return reinterpret_cast<void*>(p);
    }

    OX_INLINE void* offset_ptr(void* OX_RESTRICT ptr, const usize offset) {
        return reinterpret_cast<void*>(reinterpret_cast<uptr>(ptr) + offset);
    }


} // namespace ox

#endif //OMNIX_MEMORY_H