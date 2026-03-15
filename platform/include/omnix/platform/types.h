//
// Created by cakilgan on 3/2/26.
//

#ifndef OMNIX_TYPES_H
#define OMNIX_TYPES_H

#define OX_TYPES_USE_STDINT

#include <typeindex>

#ifdef OX_TYPES_USE_STDINT
#include <cstdint>
#endif

#include "defines.h"

namespace ox {
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
    OX_USING( loc,       i64         );
    OX_USING( basic_byte,u8          );
    OX_USING( cstr,      const char* );
    OX_USING( cchar,     const char  );
    OX_USING( dstr,      char*       );
    OX_USING( vptr,      void*       );
    OX_USING( cvptr,     const void* );
    OX_USING( vptrc,     void*const  );


    OX_USING(opq,
        struct opaque {
            void* _raw = nullptr;
            std::type_index _type{typeid(void)};
            void (*_deleter)(void*) = nullptr;
            opaque() = default;
            template<typename T>
            static opaque make(T* ptr) {
                return opaque{
                    ptr,
                    typeid(T),
                    [](vptr p) { delete static_cast<T*>(p); }
               };
            }

            ~opaque() {
                reset();
            }

            void reset() noexcept {
                if (_raw && _deleter) {
                    _deleter(_raw);
                }
                _raw = nullptr;
                _deleter = nullptr;
                _type = typeid(void);
            }

            opaque(opaque&& other) noexcept
                : _raw(other._raw),
                _type(other._type),
                _deleter(other._deleter)
            {
                other._raw = nullptr;
                other._deleter = nullptr;
                other._type = typeid(void);
            }

            opaque& operator=(opaque&& other) noexcept {
                if (this != &other) {
                    reset();
                    _raw = other._raw;
                    _type = other._type;
                    _deleter = other._deleter;

                    other._raw = nullptr;
                    other._deleter = nullptr;
                    other._type = typeid(void);
                }
                return *this;
            }

            opaque(const opaque&) = delete;
            opaque& operator=(const opaque&) = delete;

            template<typename T>
            T* as() const noexcept {
                if (_type != typeid(T))
                    return nullptr;
                return static_cast<T*>(_raw);
            }

            explicit operator bool() const noexcept {
                return _raw != nullptr;
            }

            vptr release() noexcept {
                vptr tmp = _raw;
                _raw = nullptr;
                _deleter = nullptr;
                _type = typeid(void);
                return tmp;
            }

            friend void swap(opaque& a, opaque& b) noexcept;
        private:
            opaque(void* p, const std::type_info& ti, void(*d)(void*))
                : _raw(p), _type(ti), _deleter(d) {}
        };
    );


    template<typename T = void>
    constexpr T* null = nullptr;
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
