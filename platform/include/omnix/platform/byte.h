//
// Created by cakilgan on 3/4/26.
//

#ifndef OMNIX_BYTE_H
#define OMNIX_BYTE_H
#pragma once

#include "defines.h"
#include "types.h"

#ifndef OX_BYTE_NO_STD_HELPERS
#include <bitset>
#include <iostream>
#include <string>
#endif

namespace ox {
		struct byte{
		public:
			struct _Bits {
				_Bits() = default;
#if OX_IS(ENDIAN,BIG)
				u8 b7 : 1, b6 : 1, b5 : 1, b4 : 1, b3 : 1, b2 : 1, b1 : 1, b0 : 1;
#else
				u8 b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1;
#endif
			};
		private:
			union {
				u8 value;
				_Bits Bits;
			};
		public:

			OX_FORCE_CONSTEXPR OX_FORCE_INLINE byte(u8 _Set = 0):value(_Set){}

			OX_FORCE_INLINE void set(i32 pos) { value |= (1 << pos); };
			OX_FORCE_INLINE void clear(i32 pos) { value &= ~(1 << pos); };
			OX_FORCE_INLINE void flip(i32 pos) { value ^= (1 << pos); };
			OX_FORCE_CONSTEXPR OX_FORCE_INLINE bool test(i32 pos) const { return (value >> pos) & 1; };

			OX_FORCE_CONSTEXPR OX_FORCE_INLINE byte operator|(byte other) const { return {static_cast<u8>(value | other.value)}; }
			OX_FORCE_CONSTEXPR OX_FORCE_INLINE byte operator&(byte other) const { return {static_cast<u8>(value & other.value)}; }
			OX_FORCE_CONSTEXPR OX_FORCE_INLINE byte operator^(byte other) const { return {static_cast<u8>(value ^ other.value)}; }
			OX_FORCE_CONSTEXPR OX_FORCE_INLINE byte operator<<(i32 shift) const { return {static_cast<u8>(value << shift)}; }
			OX_FORCE_CONSTEXPR OX_FORCE_INLINE byte operator>>(i32 shift) const { return {static_cast<u8>(value >> shift)}; }

			OX_FORCE_INLINE byte& operator|=(byte other) { value |= other.value; return *this; };
			OX_FORCE_INLINE byte& operator&=(byte other) { value &= other.value; return *this; };
			OX_FORCE_INLINE byte& operator^=(byte other) { value ^= other.value; return *this; };
			OX_FORCE_INLINE byte& operator<<=(i32 shift) { value <<= shift; return *this; };
			OX_FORCE_INLINE byte& operator>>=(i32 shift) { value >>= shift; return *this; };

			OX_FORCE_CONSTEXPR OX_FORCE_INLINE byte operator~() const { return byte(static_cast<u8>(~value)); };
			OX_FORCE_CONSTEXPR OX_FORCE_INLINE bool operator==(byte other) const { return value == other.value; };
			OX_FORCE_CONSTEXPR OX_FORCE_INLINE bool operator!=(byte other) const { return value != other.value; };

			OX_FORCE_CONSTEXPR OX_FORCE_INLINE u8 to_u8() const { return value; }

			OX_FORCE_INLINE _Bits to_bits() const { return Bits; }
			OX_FORCE_INLINE _Bits& bits() { return Bits; };

#ifndef OX_BYTE_NO_STD_HELPERS
			OX_FORCE_INLINE std::string to_binary() const {
				return std::bitset<8>(value).to_string();
			}
			OX_FORCE_INLINE friend std::ostream& operator<<(std::ostream& os, const byte& b) {
				os << "0x" << std::hex << static_cast<int>(b.value) << std::dec << " (" << b.to_binary() << ")";
				return os;
			}
#endif
		};

		OX_STATIC_ASSERT(sizeof(::ox::byte) == 1, "size of omnix#byte must be exactly 1 byte.");
		OX_USING(bytes_t, i64);

		struct bytes {
		    bytes_t ct;

		    OX_FORCE_CONSTEXPR bytes() : ct(0) {}
		    OX_FORCE_CONSTEXPR bytes(bytes_t b) : ct(b) {}

		    OX_FORCE_CONSTEXPR operator bytes_t() const { return ct; }
		    OX_FORCE_CONSTEXPR operator bytes_t&()       { return ct; }

		    OX_FORCE_CONSTEXPR bool operator==(const bytes& rhs) const { return ct == rhs.ct; }
		    OX_FORCE_CONSTEXPR bool operator!=(const bytes& rhs) const { return ct != rhs.ct; }
		    OX_FORCE_CONSTEXPR bool operator< (const bytes& rhs) const { return ct <  rhs.ct; }
		    OX_FORCE_CONSTEXPR bool operator<=(const bytes& rhs) const { return ct <= rhs.ct; }
		    OX_FORCE_CONSTEXPR bool operator> (const bytes& rhs) const { return ct >  rhs.ct; }
		    OX_FORCE_CONSTEXPR bool operator>=(const bytes& rhs) const { return ct >= rhs.ct; }

		    OX_FORCE_CONSTEXPR bytes& operator+=(const bytes& rhs) { ct += rhs.ct; return *this; }
		    OX_FORCE_CONSTEXPR bytes& operator-=(const bytes& rhs) { ct -= rhs.ct; return *this; }
		    OX_FORCE_CONSTEXPR bytes& operator*=(bytes_t scalar)   { ct *= scalar;  return *this; }
		    OX_FORCE_CONSTEXPR bytes& operator/=(bytes_t scalar)   { ct /= scalar;  return *this; }
		};

		OX_FORCE_CONSTEXPR OX_INLINE bytes   gigabytes(const bytes_t gb)    { return gb * 1024*1024*1024LL; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes_t gigabytes(const bytes& gb)		{ return gb.ct / (1024LL*1024*1024); }
		OX_FORCE_CONSTEXPR OX_INLINE bytes   megabytes(const bytes_t mb)    { return mb * 1024LL*1024; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes_t megabytes(const bytes& mb)		{ return mb.ct / (1024LL*1024); }
		OX_FORCE_CONSTEXPR OX_INLINE bytes   kilobytes(const bytes_t kb)    { return kb * 1024LL; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes_t kilobytes(const bytes& kb)		{ return kb.ct / 1024LL; }

		OX_FORCE_CONSTEXPR OX_INLINE bytes operator-(const bytes& a, const bytes& b) { return a.ct - b.ct; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator+(const bytes& a, const bytes& b) { return a.ct + b.ct; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator/(const bytes& a, const bytes& b) { return a.ct / b.ct; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator*(const bytes& a, const bytes& b) { return a.ct * b.ct; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator%(const bytes& a, const bytes& b) { return a.ct % b.ct; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator&(const bytes& a, const bytes& b) { return a.ct & b.ct; }

		OX_FORCE_CONSTEXPR OX_INLINE bytes operator-(const loc&  a, const bytes& b) { return a - b.ct; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator+(const loc&  a, const bytes& b) { return a + b.ct; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator/(const loc&  a, const bytes& b) { return a / b.ct; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator*(const loc&  a, const bytes& b) { return a * b.ct; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator%(const loc&  a, const bytes& b) { return a % b.ct; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator&(const loc&  a, const bytes& b) { return a & b.ct; }

		OX_FORCE_CONSTEXPR OX_INLINE bytes operator-(const bytes& a, const loc& b) { return a.ct - b; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator+(const bytes& a, const loc& b) { return a.ct + b; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator/(const bytes& a, const loc& b) { return a.ct / b; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator*(const bytes& a, const loc& b) { return a.ct * b; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator%(const bytes& a, const loc& b) { return a.ct % b; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator&(const bytes& a, const loc& b) { return a.ct & b; }

		OX_FORCE_CONSTEXPR OX_INLINE bytes operator*(const bytes& a, const i64 scalar) { return a.ct * scalar; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator/(const bytes& a, const i64 scalar) { return a.ct / scalar; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator*(const i64 scalar, const bytes& a) { return a.ct * scalar; }
		OX_FORCE_CONSTEXPR OX_INLINE bytes operator/(const i64 scalar, const bytes& a) { return scalar / a.ct; }
}
#endif //OMNIX_BYTE_H