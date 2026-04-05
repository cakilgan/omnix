//
// Created by cakilgan on 3/18/26.
//

#ifndef OMNIX_BYTE_H
#define OMNIX_BYTE_H
#pragma once
#ifndef OX_BYTE_NO_STD_HELPERS
#include <bitset>
#include <iostream>
#include <string>
#endif

#include "../platform/assert.h"
#include "defines.h"
#include "types.h"
#include "constants.h"
#include "loc.h"

namespace ox {

    struct byte {
        u8 value;
    };

    OX_FORCE_INLINE void bit_set(byte& b, i32 pos)    { b.value |= (1 << pos); }
    OX_FORCE_INLINE void bit_clear(byte& b, i32 pos)  { b.value &= ~(1 << pos); }
    OX_FORCE_INLINE void bit_flip(byte& b, i32 pos)   { b.value ^= (1 << pos); }
    OX_FORCE_CONSTEXPR OX_FORCE_INLINE bool bit_test(const byte& b, i32 pos) { return (b.value >> pos) & 1; }

#ifndef OX_BYTE_NO_STD_HELPERS
    OX_FORCE_INLINE std::string to_binary(const byte& x){
        return std::bitset<8>(x.value).to_string();
    }
    OX_FORCE_INLINE std::ostream& operator<<(std::ostream& os, const byte& b) {
        os << "0x" << std::hex << static_cast<int>(b.value) << std::dec << " (" << to_binary(b) << ")";
        return os;
    }
#endif

    OX_STATIC_ASSERT(sizeof(byte) == 1, "ox::byte must be exactly 1 byte");

    OX_USING(bytes_t, u64);

    struct bytes {
        bytes_t ct;

        OX_FORCE_CONSTEXPR bytes() : ct(0) {}
        explicit OX_FORCE_CONSTEXPR bytes(bytes_t b) : ct(b) {}
        explicit OX_FORCE_CONSTEXPR bytes(loc b) : ct(b.value) {}

        OX_FORCE_CONSTEXPR bool operator==(const bytes& rhs) const { return ct == rhs.ct; }
        OX_FORCE_CONSTEXPR bool operator!=(const bytes& rhs) const { return ct != rhs.ct; }
        OX_FORCE_CONSTEXPR bool operator< (const bytes& rhs) const { return ct <  rhs.ct; }
        OX_FORCE_CONSTEXPR bool operator<=(const bytes& rhs) const { return ct <= rhs.ct; }
        OX_FORCE_CONSTEXPR bool operator> (const bytes& rhs) const { return ct >  rhs.ct; }
        OX_FORCE_CONSTEXPR bool operator>=(const bytes& rhs) const { return ct >= rhs.ct; }

        OX_FORCE_CONSTEXPR bytes& operator+=(const bytes& rhs) { ct += rhs.ct; return *this; }
        OX_FORCE_CONSTEXPR bytes& operator-=(const bytes& rhs) { ct -= rhs.ct; return *this; }
        OX_FORCE_CONSTEXPR bytes& operator*=(bytes_t scalar) { ct *= scalar; return *this; }
        OX_FORCE_CONSTEXPR bytes& operator/=(bytes_t scalar) { ct /= scalar; return *this; }

        OX_FORCE_CONSTEXPR bytes operator-(const bytes& rhs) const { return bytes(ct - rhs.ct);}
        OX_FORCE_CONSTEXPR bytes operator+(const bytes& rhs) const { return bytes(ct + rhs.ct);}
        OX_FORCE_CONSTEXPR usize operator/(const bytes& rhs) const { return static_cast<usize>(ct / rhs.ct);    }
        OX_FORCE_CONSTEXPR bytes operator%(const bytes& rhs) const { return bytes(ct % rhs.ct);}
    };

    OX_FORCE_CONSTEXPR OX_INLINE bytes gigabytes(const bytes_t gb) { return bytes(gb * 1024LL * 1024LL * 1024LL); }
    OX_FORCE_CONSTEXPR OX_INLINE bytes_t gigabytes(const bytes& gb) { return gb.ct / (1024LL*1024*1024); }

    OX_FORCE_CONSTEXPR OX_INLINE bytes megabytes(const bytes_t mb) { return bytes(mb * 1024LL * 1024LL); }
    OX_FORCE_CONSTEXPR OX_INLINE bytes_t megabytes(const bytes& mb) { return mb.ct / (1024LL*1024); }

    OX_FORCE_CONSTEXPR OX_INLINE bytes kilobytes(const bytes_t kb) { return bytes(kb * 1024LL); }
    OX_FORCE_CONSTEXPR OX_INLINE bytes_t kilobytes(const bytes& kb) { return kb.ct / 1024LL; }

    OX_FORCE_CONSTEXPR OX_INLINE bytes operator*(const bytes& a, const u64 scalar) { return bytes(a.ct * scalar); }
    OX_FORCE_CONSTEXPR OX_INLINE bytes operator*(const u64 scalar, const bytes& a) { return bytes(a.ct * scalar); }
    OX_FORCE_CONSTEXPR OX_INLINE bytes operator/(const bytes& a, const u64 scalar) { return bytes(a.ct / scalar); }

    OX_FORCE_CONSTEXPR OX_INLINE byte* operator+(byte* ptr, const bytes& add) { return ptr + add.ct; }
    OX_FORCE_CONSTEXPR OX_INLINE byte* operator-(byte* ptr, const bytes& add) { return ptr - add.ct; }
    OX_FORCE_CONSTEXPR OX_INLINE const byte* operator+(const byte* ptr, const bytes& add) { return ptr + add.ct; }
    OX_FORCE_CONSTEXPR OX_INLINE const byte* operator-(const byte* ptr, const bytes& add) { return ptr - add.ct; }

    OX_FORCE_CONSTEXPR OX_INLINE loc operator+(loc l, const bytes& b) {
        OX_ASSERT(b.ct <= (u64_max - l.value));
        return loc{l.value + b.ct};
    }
    OX_FORCE_CONSTEXPR OX_INLINE loc operator-(loc l, const bytes& b) {
        OX_ASSERT(l.value >= b.ct);
        return loc{l.value - b.ct};
    }
    OX_FORCE_CONSTEXPR loc& operator+=(loc& l, const bytes& b) {
        l = l + b;
        return l;
    }
    OX_FORCE_CONSTEXPR loc& operator-=(loc& l, const bytes& b) {
        l = l - b;
        return l;
    }
} // namespace ox

#define OX_BYTES(x) ::ox::bytes(x)
#define OX_KB(x) ::ox::kilobytes(x)
#define OX_MB(x) ::ox::megabytes(x)
#define OX_GB(x) ::ox::gigabytes(x)

#endif // OMNIX_BYTE_H