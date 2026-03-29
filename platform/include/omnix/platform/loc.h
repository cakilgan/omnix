//
// Created by cakilgan on 3/19/26.
//

#ifndef OMNIX_LOC_H
#define OMNIX_LOC_H

#include "types.h"
#include "constants.h"

namespace ox{
    namespace types {
        struct loc {
            u64 value;
            explicit OX_FORCE_CONSTEXPR loc(const u64 v = 0x0) : value(v) {}
            OX_FORCE_CONSTEXPR OX_INLINE loc &operator++() {
                value++;
                return *this;
            }
            OX_FORCE_CONSTEXPR OX_INLINE loc operator++(int _dummy) {
                auto old_v = value;
                value++;
                return loc{old_v};
            }

        };
        OX_FORCE_CONSTEXPR bool operator==(loc a, loc b) { return a.value == b.value; }
        OX_FORCE_CONSTEXPR bool operator!=(loc a, loc b) { return a.value != b.value; }
        OX_FORCE_CONSTEXPR bool operator< (loc a, loc b) { return a.value <  b.value; }
        OX_FORCE_CONSTEXPR bool operator<=(loc a, loc b) { return a.value <= b.value; }
        OX_FORCE_CONSTEXPR bool operator> (loc a, loc b) { return a.value >  b.value; }
        OX_FORCE_CONSTEXPR bool operator>=(loc a, loc b) { return a.value >= b.value; }
        OX_FORCE_CONSTEXPR OX_INLINE loc operator+(loc a, loc b) {
            return loc{ a.value + b.value };
        }

        struct sloc {
            i64 value;
            explicit OX_FORCE_CONSTEXPR sloc(const i64 v = 0x0) : value(v){}
            OX_FORCE_CONSTEXPR OX_INLINE sloc &operator++() {
                value++;
                return *this;
            }
            OX_FORCE_CONSTEXPR OX_INLINE sloc operator++(int _dummy) {
                auto old_v = value;
                value++;
                return sloc{old_v};
            }
        };
        OX_FORCE_CONSTEXPR bool operator==(sloc a, sloc b) { return a.value == b.value; }
        OX_FORCE_CONSTEXPR bool operator!=(sloc a, sloc b) { return a.value != b.value; }
        OX_FORCE_CONSTEXPR bool operator< (sloc a, sloc b) { return a.value <  b.value; }
        OX_FORCE_CONSTEXPR bool operator<=(sloc a, sloc b) { return a.value <= b.value; }
        OX_FORCE_CONSTEXPR bool operator> (sloc a, sloc b) { return a.value >  b.value; }
        OX_FORCE_CONSTEXPR bool operator>=(sloc a, sloc b) { return a.value >= b.value; }
        OX_FORCE_CONSTEXPR OX_INLINE sloc operator+(sloc a, sloc b) {
            return sloc{ a.value + b.value };
        }
        OX_FORCE_CONSTEXPR OX_INLINE sloc operator-(sloc a, sloc b) {
            return sloc{ a.value - b.value };
        }
    }

    namespace constants {
        OX_FORCE_CONSTEXPR loc  loczero   {0x0};
        OX_FORCE_CONSTEXPR loc  locinvalid   {u64_max};
        OX_FORCE_CONSTEXPR sloc sloczero {0x0};
        OX_FORCE_CONSTEXPR sloc slocinvalid {i64_max};
    }

    using ox::constants::loczero;
    using ox::constants::locinvalid;
    using ox::constants::sloczero;
    using ox::constants::slocinvalid;
}


#endif //OMNIX_LOC_H
