#pragma once
#ifndef INCLUDE_GUARD_OMNIXLIB_DETECTS_HEADER_
#define INCLUDE_GUARD_OMNIXLIB_DETECTS_HEADER_

#include <oxlib/macros.h>

#if OX_IS_UNDEFINED(OX_PLATFORM)
#undef OX_PLATFORM
#if defined(_WIN32)
#define OX_PLATFORM OX_PLATFORM_WINDOWS
#endif
#if defined(__linux__) && !defined(__android__)
#define OX_PLATFORM OX_PLATFORM_LINUX
#endif
#if defined(__APPLE__)
#define OX_PLATFORM OX_PLATFORM_APPLE
#endif
#if defined(__android__)
#define OX_PLATFORM OX_PLATFORM_ANDROID
#endif
#endif

#if OX_IS_UNDEFINED(OX_TC)
#undef OX_TC

#if defined(__clang__)
#if defined(_MSC_VER)
#define OX_TC OX_CLANG_CL
#else
#define OX_TC OX_CLANG
#endif

#elif defined(_MSC_VER)
#define OX_TC OX_MSVC

#elif defined(__GNUC__)
#define OX_TC OX_GCC

#elif defined(__clang__)
#define OX_TC OX_CLANG
#endif

#endif

#if OX_IS_UNDEFINED(OX_CXX_STD)
#undef OX_CXX_STD
#if OX_TC == OX_MSVC
#define OX_CXX_STD _MSVC_LANG
#else
#define OX_CXX_STD __cplusplus
#endif
#endif

#if OX_IS_UNDEFINED(OX_CXX)
#undef OX_CXX
#if OX_CXX_STD == 199711L
#define OX_CXX OX_CXX_98
#endif
#if OX_CXX_STD == 201103L
#define OX_CXX OX_CXX_11
#endif
#if OX_CXX_STD == 201402L
#define OX_CXX OX_CXX_14
#endif
#if OX_CXX_STD == 201703L
#define OX_CXX OX_CXX_17
#endif
#if OX_CXX_STD == 202002L
#define OX_CXX OX_CXX_20
#endif
#if OX_CXX_STD == 202302L
#define OX_CXX OX_CXX_23
#endif
#if OX_CXX_STD > 202302L
#define OX_CXX OX_CXX_26
#endif
#endif

#define OX_STRINGIFY_HELPER(x) #x
#define OX_STRINGIFY(x) OX_STRINGIFY_HELPER(x)

#if OX_IS_UNDEFINED(OX_INTERNAL_WARNING("NULL"))
#undef OX_INTERNAL_WARNING
#if OX_TC == OX_MSVC
#define OX_INTERNAL_WARNING(msg)                                               \
  __pragma(message(__FILE__ "(" OX_STRINGIFY(__LINE__) "): warning: " msg))
#else
#define OX_INTERNAL_WARNING(msg) _Pragma(OX_STRINGIFY(GCC warning msg))
#endif
#endif

#if OX_IS_UNDEFINED(OX_MODE)
#undef OX_MODE
#ifdef _DEBUG
#define OX_MODE OX_MODE_DEBUG
#else
#define OX_MODE OX_MODE_RELEASE
#endif
#endif

#if OX_IS_UNDEFINED(OX_HALT())
#undef OX_HALT
#if OX_MODE == OX_MODE_DEBUG
#if defined(_MSC_VER)
#define OX_HALT() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define OX_HALT() __builtin_trap()
#else
#include <stdlib.h>
#define OX_HALT() abort()
#endif
#else
#endif
#endif

#if OX_IS_UNDEFINED(OX_ASSERTMSG(false, "NULL")) &&                            \
    OX_IS_UNDEFINED(OX_ASSERT(false))
#undef OX_ASSERTMSG
#undef OX_ASSERT
#include <cstdio>
OX_NAMESPACEDEF {
  inline void internal_log_assert(const char *cond, const char *file, int line,
                                  const char *msg) {
    if (msg) {
      std::printf("assertion$/: %s is failed. file %s line: %i \n msg: %s\n",
                  cond, file, line, msg);
    } else {
      std::printf("assertion$/: %s is failed. file %s line: %i\n", cond, file,
                  line);
    }
  }
  template <typename... Args>
  inline void internal_log_assert_f(const char *format, Args... values) {
    std::printf(format, values...);
  }
}

#define OX_ASSERTMSG(expr, fmt, ...)                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      OX_NAMESPACE::internal_log_assert_f(                                     \
          "assertion: %s failed. file %s line %d " fmt, #expr, __FILE__,       \
          __LINE__, ##__VA_ARGS__);                                            \
      OX_HALT();                                                               \
    }                                                                          \
  } while (0)

#define OX_ASSERT(expr) OX_ASSERTMSG(expr, "")

#endif

#if OX_IS_UNDEFINED(OX_INLINE_VARS_SUPPORTED)
#undef OX_INLINE_VARS_SUPPORTED
#define OX_INLINE_VARS_SUPPORTED _INLINE_VARIABLES_SUPPORTED
#endif

#ifndef OX_PLATFORM
#error "OX_PLATFORM is undefined."
#endif
#ifndef OX_TC
#error "OX_TC is undefined."
#endif
#ifndef OX_CXX_STD
#error "OX_CXX STANDARD is undefined."
#endif
#ifndef OX_CXX
#error "OX_CXX is undefined."
#endif

#endif
