#pragma once
#ifndef INCLUDE_GUARD_OMNIXLIB_MACROS_HEADER_
#define INCLUDE_GUARD_OMNIXLIB_MACROS_HEADER_

#define OX_NAME "OmniX"

#define OX_UNDEFINED -2
#define OX_ERROR -1
#define OX_IS_UNDEFINED(X) (X == OX_UNDEFINED)
#define OX_IS_ERROR(X) (X == OX_ERROR)
#define OX_IS_USABLE(X) !(OX_IS_UNDEFINED(X) || OX_IS_ERROR(X))

#define OX_EXPLICIT_USING(T, Name) using Name = T
#define OX_EXPLICIT_TYPEDEF(T, Name) typedef T Name

#define OX_STATIC_ASSERT(cond, msg) static_assert(cond, msg)

#ifdef OXLIBCONFIG_SET
#if OXLIBCONFIG_USE_EXPLICIT_TYPEDEF
#define OX_TYPEDEF(T, Name) OX_EXPLICIT_TYPEDEF(T, Name)
#else
#define OX_TYPEDEF(T, Name) OX_EXPLICIT_USING(T, Name)
#endif
#if OXLIBCONFIG_DONT_USE_OX_NAMESPACE
#define OX_NAMESPACEDEF namespace
#define OX_NAMESPACE
#else
#define OX_NAMESPACEDEF namespace ox
#define OX_NAMESPACE ox
#endif

#if OXLIBCONFIG_USE_NESTED_NAMESPACES
#define OX_PUSH_NAMESPACE(name)                                                \
  namespace OX_NAMESPACE {                                                     \
  namespace name {
#define OX_POP_NAMESPACE                                                       \
  }                                                                            \
  }
#else
#define OX_PUSH_NAMESPACE(name) namespace OX_NAMESPACE {
#define OX_POP_NAMESPACE }
#endif

#else
#define OX_TYPEDEF(T, Name) OX_EXPLICIT_USING(T, Name)
#define OX_NAMESPACEDEF namespace ox
#define OX_NAMESPACE ox
#endif

#define OX_PLATFORM_WINDOWS 0
#define OX_PLATFORM_LINUX 1
#define OX_PLATFORM_ANDROID 2
#define OX_PLATFORM_APPLE 3
#define OX_PLATFORM OX_UNDEFINED

#define OX_MSVC 0
#define OX_CLANG 1
#define OX_GCC 2
#define OX_CLANG_CL 3
#define OX_TC OX_UNDEFINED

#define OX_CXX_98 0
#define OX_CXX_11 1
#define OX_CXX_14 2
#define OX_CXX_17 3
#define OX_CXX_20 4
#define OX_CXX_23 5
#define OX_CXX_26 6
#define OX_CXX_STD OX_UNDEFINED
#define OX_CXX OX_UNDEFINED

#define OX_MODE_DEBUG 0
#define OX_MODE_RELEASE 1
#define OX_MODE OX_UNDEFINED

#define OX_INTERNAL_WARNING(msg) OX_UNDEFINED
#define OX_HALT() OX_UNDEFINED
#define OX_ASSERT(expression) OX_UNDEFINED
#define OX_ASSERTMSG(expression, Msg) OX_UNDEFINED
#define OX_INLINE_VARS_SUPPORTED OX_UNDEFINED

/*
Macros::
OX_PLATFORM
OX_TC
OX_CXX
OX_CXX_STD
OX_MODE (build mode)
*/
#endif
