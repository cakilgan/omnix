//
// Created by cakilgan on 3/28/26.
//

#ifndef OMNIX_HASH_H
#define OMNIX_HASH_H
#include <omnix/platform/types.h>

#include "../core/string.h"
#include "strview.h"
namespace ox {
    template<typename T>
    struct hash {
        usize operator()(const T& value) const {
            return value.hash();
        }
    };

    template<>
        struct hash<u64> {
        usize operator()(u64 x) const {
            x ^= x >> 33;
            x *= 0xff51afd7ed558ccdULL;
            x ^= x >> 33;
            x *= 0xc4ceb9fe1a85ec53ULL;
            x ^= x >> 33;
            return static_cast<usize>(x);
        }
    };

    template<>
    struct hash<i32> {
        usize operator()(i32 x) const {
            return hash<u64>{}(static_cast<u64>(static_cast<i64>(x)));
        }
    };

    template<>
        struct hash<strview> {
        constexpr usize operator()(const strview& s) const {
            usize hash = 1469598103934665603ULL;

            for (usize i = 0; i != s.len; ++i) {
                hash ^= static_cast<u8>(s.data[i]);
                hash *= 1099511628211ULL;
            }

            return hash;
        }
    };

    template<typename T>
    struct hash<T*> {
        usize operator()(T* ptr) const {
            return hash<u64>{}(reinterpret_cast<uptr>(ptr));
        }
    };

    template<>
    struct hash<cstr> {
        usize operator()(cstr s) const {
            usize hash = 14695981039346656037ULL;

            while (*s) {
                hash ^= static_cast<u8>(*s++);
                hash *= 1099511628211ULL;
            }

            return hash;
        }
    };

    template<>
    struct hash<bool> {
        usize operator()(bool v) const {
            return v ? 0x9e3779b97f4a7c15ULL : 0;
        }
    };
    template<>
    struct hash<str> {
        usize operator()(str v) const {
            return hash<strview>{}(v.data());
        }
    };

    inline void hash_combine(usize& seed, usize value) {
        seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    }
}
#endif //OMNIX_HASH_H