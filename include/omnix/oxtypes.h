#pragma once
#include <omnix/oxmacros.h>

namespace OX_NAMESPACE {
    OX_TYPEDEF(char,i8);
    OX_TYPEDEF(short,i16);
    OX_TYPEDEF(int,i32);
    OX_TYPEDEF(long long,i64);

    OX_TYPEDEF(unsigned char,u8);
    OX_TYPEDEF(unsigned short,u16);
    OX_TYPEDEF(unsigned int,u32);
    OX_TYPEDEF(unsigned long long,u64);

    OX_TYPEDEF(float, f32);
    OX_TYPEDEF(double, f64);

    OX_TYPEDEF(u8, boolean);
    OX_TYPEDEF(u8, byte);

    //WARNING: opaque pointer handle.
    struct opaque;
    OX_TYPEDEF(opaque*,vptr);
    
    OX_TYPEDEF(const char*, cstr);
}


#define SASIZE(x,size) static_assert(sizeof(x)==size,"size of" #x " must be equal to " #size)

SASIZE(ox::i8,1);
SASIZE(ox::i16,2);
SASIZE(ox::i32,4);
SASIZE(ox::i64,8);

SASIZE(ox::u8, 1);
SASIZE(ox::u16, 2);
SASIZE(ox::u32, 4);
SASIZE(ox::u64, 8);

SASIZE(ox::f32,4);
SASIZE(ox::f64,8);

SASIZE(ox::byte,1);
SASIZE(ox::boolean,1);

#undef SASIZE