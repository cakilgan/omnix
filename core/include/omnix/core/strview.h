//
// Created by cakilgan on 3/28/26.
//

#ifndef OMNIX_STRVIEW_H
#define OMNIX_STRVIEW_H
#include <omnix/platform/types.h>
#include <omnix/platform/util.h>
namespace ox {
    struct strview {
        cstr data {nullptr};
        usize len {0};
        template<size_t N>
        constexpr strview(const char (&arr)[N])
        : data(arr), len(N-1) {}

        constexpr strview(const char* arr)
        : data(arr), len(strlen(arr)) {}

        constexpr strview():data(nullptr),len(0){}

        constexpr bool operator==(const strview& other) const {
            if (len != other.len) return false;
            for (usize i = 0; i < len; ++i) {
                if (data[i] != other.data[i]) return false;
            }
            return true;
        }
    };
}
#endif //OMNIX_STRVIEW_H