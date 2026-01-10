#pragma once
#ifndef INCLUDE_GUARD_OMNIXLIB_TYPES_HEADER_
#define INCLUDE_GUARD_OMNIXLIB_TYPES_HEADER_

#include <oxlib/detects.h>
#include <oxlib/macros.h>

#ifndef OXLIBCONFIG_SET
OX_INTERNAL_WARNING("config is not set.")
#endif

OX_NAMESPACEDEF {
  OX_TYPEDEF(char, i8);
  OX_TYPEDEF(short, i16);
  OX_TYPEDEF(int, i32);
  OX_TYPEDEF(long long, i64);

  OX_TYPEDEF(unsigned char, u8);
  OX_TYPEDEF(unsigned short, u16);
  OX_TYPEDEF(unsigned int, u32);
  OX_TYPEDEF(unsigned long long, u64);

  OX_TYPEDEF(float, f32);
  OX_TYPEDEF(double, f64);

  OX_TYPEDEF(const char *, cstr);
  OX_TYPEDEF(char *, dstr);

  OX_TYPEDEF(u8, byte);

  struct opaque;
  OX_TYPEDEF(opaque *, vptr);
  OX_TYPEDEF(void *, lvptr);

  template<typename T = void>
  inline vptr asvptr(T* _Ptr){
    return dynamic_cast<vptr>(_Ptr);
  }

  template <typename T = void> inline constexpr T *null() noexcept {
    return static_cast<T *>(nullptr);
  }
};

#define CHECK(x, size)                                                         \
  static_assert(sizeof(x) == size,                                             \
                "#" OX_NAME ": sizeof> " #x "must be : " #size)
CHECK(OX_NAMESPACE::i8, 1);
CHECK(OX_NAMESPACE::i16, 2);
CHECK(OX_NAMESPACE::i32, 4);
CHECK(OX_NAMESPACE::i64, 8);
CHECK(OX_NAMESPACE::u8, 1);
CHECK(OX_NAMESPACE::u16, 2);
CHECK(OX_NAMESPACE::u32, 4);
CHECK(OX_NAMESPACE::u64, 8);
CHECK(OX_NAMESPACE::byte, 1);
#undef CHECK

#endif
