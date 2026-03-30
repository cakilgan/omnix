//
// Created by cakilgan on 3/2/26.
//

#ifndef OMNIX_TYPES_H
#define OMNIX_TYPES_H

#ifdef OX_TYPES_USE_STDINT
#include <cstdint>
#endif

#include "defines.h"

namespace ox {
    namespace types {
#ifdef OX_TYPES_USE_STDINT
        OX_USING( i8, ::std::int8_t  );
        OX_USING( i16,::std::int16_t );
        OX_USING( i32,::std::int32_t );
        OX_USING( i64,::std::int64_t );

        OX_USING( u8, ::std::uint8_t  );
        OX_USING( u16,::std::uint16_t );
        OX_USING( u32,::std::uint32_t );
        OX_USING( u64,::std::uint64_t );

        OX_USING( uptr,::std::uintptr_t      );
        OX_USING( iptr,::std::intptr_t       );
#else
        OX_USING( i8, signed char  );
        OX_USING( i16,signed short );
        OX_USING( i32,signed int   );

        OX_USING( u8, unsigned char      );
        OX_USING( u16,unsigned short     );
        OX_USING( u32,unsigned int       );

#if __WORDSIZE == 64
        OX_USING( u64,unsigned long int  );
        OX_USING( i64,signed long int    );
#else
        OX_USING( u64,unsigned long long );
        OX_USING( i64,signed long int    );
#endif

        OX_USING(uptr,u64);
        OX_USING(iptr,i64);
#endif

        OX_USING( f32, float       );
        OX_USING( f64, double      );
        OX_USING( f128,long double );

        OX_USING( usize,     u64         );
        OX_USING( isize,     i64         );

        struct loc;
        struct sloc;

        OX_USING( basic_byte,u8          );
        OX_USING( cstr,      const char* );
        OX_USING( cchar,     const char  );
        OX_USING( dstr,      char*       );
        OX_USING( arrstr,    char**      );
        OX_USING( vptr,      void*       );
        OX_USING( cvptr,     const void* );
        OX_USING( vptrc,     void*const  );

        using type_id = u64;
        namespace impl {
            inline u64 next_type_id() {
                static u64 counter = 0;
                return ++counter;
            }
        };

        template<typename T>
        type_id type_of() {
            static type_id id = impl::next_type_id();
            return id;
        }
        template<typename T>
        type_id type_of(const T& _) {
            return type_of<T>();
        }


        struct opaque;
        using opq = opaque;

        template<typename T = void>
        constexpr T* null = nullptr;
    }

    using types::i8;
    using types::i16;
    using types::i32;
    using types::i64;

    using types::u8;
    using types::u16;
    using types::u32;
    using types::u64;

    using types::uptr;
    using types::iptr;

    using types::f32;
    using types::f64;
    using types::f128;

    using types::isize;
    using types::usize;

    using types::loc;
    using types::sloc;

    using types::basic_byte;
    using types::cstr;
    using types::dstr;
    using types::arrstr;

    using types::cchar;
    using types::vptr;
    using types::cvptr;
    using types::vptrc;

    using types::opq;
    using types::opaque;

    using types::null;
}

OX_STATIC_ASSERT( sizeof(::ox::i8)  == 1,  "size of omnix#i8  must be exactly 1 byte."  );
OX_STATIC_ASSERT( sizeof(::ox::i16) == 2,  "size of omnix#i16 must be exactly 2 bytes." );
OX_STATIC_ASSERT( sizeof(::ox::i32) == 4,  "size of omnix#i32 must be exactly 4 bytes." );
OX_STATIC_ASSERT( sizeof(::ox::i64) == 8,  "size of omnix#i64 must be exactly 8 bytes." );

OX_STATIC_ASSERT( sizeof(::ox::u8)  == 1,  "size of omnix#u8  must be exactly 1 byte."  );
OX_STATIC_ASSERT( sizeof(::ox::u16) == 2,  "size of omnix#u16 must be exactly 2 bytes." );
OX_STATIC_ASSERT( sizeof(::ox::u32) == 4,  "size of omnix#u32 must be exactly 4 bytes." );
OX_STATIC_ASSERT( sizeof(::ox::u64) == 8,  "size of omnix#u64 must be exactly 8 bytes." );

OX_STATIC_ASSERT( sizeof(::ox::basic_byte) == 1, "size of omnix#basic_byte  must be exactly 1 byte." );

#endif //OMNIX_TYPES_H
