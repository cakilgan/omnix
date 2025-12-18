#pragma once
#include <omnix/oxconfig.h>
#include <omnix/oxmacros.h>
#include <omnix/oxtypes.h>

namespace OX_NAMESPACE{
    OX_CONSTEXPR ox::f64 pi = 3.14159265358979323846;
    OX_CONSTEXPR ox::f64 pi_divb_2 = 1.57079632679489661923;
    OX_CONSTEXPR ox::f64 pi_divb_4 = 0.785398163397448309616;
    OX_CONSTEXPR ox::f64 one_divb_pi = 0.318309886183790671538;
    OX_CONSTEXPR ox::f64 two_divb_pi = 0.636619772367581343076; 
    OX_CONSTEXPR ox::f64 two_divb_sqrt_of_pi = 1.12837916709551257390;
    OX_CONSTEXPR ox::f64 sqrt_of_2 = 1.41421356237309504880;
    OX_CONSTEXPR ox::f64 e = 2.71828182845904523536;

    OX_CONSTEXPR ox::u8 u_min = 0x0;

    OX_CONSTEXPR ox::u8 u8_max = 0xff;
    OX_CONSTEXPR ox::u16 u16_max = 0xffff;
    OX_CONSTEXPR ox::u32 u32_max = 0xffffffff;
    OX_CONSTEXPR ox::u64 u64_max = 0xffffffffffffffff;

    OX_CONSTEXPR ox::i8 i8_max = 127;
    OX_CONSTEXPR ox::i16 i16_max = 32767;
    OX_CONSTEXPR ox::i32 i32_max = 2147483647;
    OX_CONSTEXPR ox::i64 i64_max = 9223372036854775807;

    OX_CONSTEXPR ox::i8 i8_min = (-127 - 1);
    OX_CONSTEXPR ox::i16 i16_min = (-32767 - 1);
    OX_CONSTEXPR ox::i32 i32_min = (-2147483647 - 1);
    OX_CONSTEXPR ox::i64 i64_min = (-9223372036854775807 - 1);

    OX_CONSTEXPR auto null = nullptr;
}