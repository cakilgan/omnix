//
// Created by cakilgan on 3/28/26.
//

#ifndef OMNIX_STRVIEW_H
#define OMNIX_STRVIEW_H
#include <omnix/platform/types.h>
#include <omnix/platform/util.h>
namespace ox {
    template<typename T>
    struct string_view {
        const T* data {nullptr};
        usize len {0};

        template<usize N>
        constexpr string_view(const T (&arr)[N])
        : data(arr), len(N-1) {}

        constexpr string_view(const T* arr)
        : data(arr), len(strlen(arr)) {}

        constexpr string_view(const T* arr,usize len)
        : data(arr), len(len) {}

        constexpr string_view():data(nullptr),len(0){}

        constexpr bool operator==(const string_view& other) const {
            if (len != other.len) return false;
            for (usize i = 0; i < len; ++i) {
                if (data[i] != other.data[i]) return false;
            }
            return true;
        }
    };
    using strview   = string_view<char>;
    using str16view = string_view<char16_t>;
    using str32view = string_view<char32_t>;
}
#endif //OMNIX_STRVIEW_H