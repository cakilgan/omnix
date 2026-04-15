//
// Created by cakilgan on 3/1/26.
//

#ifndef OMNIX_DETECT_H
#define OMNIX_DETECT_H

#include "macros.h"

// ==================== C++ Standard Detection ====================
#if __cplusplus >= 202611L
#define OX_STANDARD OX_STANDARD_26
#elif __cplusplus >= 202311L
#define OX_STANDARD OX_STANDARD_23
#elif __cplusplus >= 202002L
#define OX_STANDARD OX_STANDARD_20
#elif __cplusplus >= 201703L
#define OX_STANDARD OX_STANDARD_17
#elif __cplusplus >= 201402L
#define OX_STANDARD OX_STANDARD_14
#else
#define OX_STANDARD OX_STANDARD_11
#endif

// ==================== Compiler Detection ====================
#if defined(__clang__)
#define OX_COMPILER OX_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
#define OX_COMPILER OX_COMPILER_GCC
#elif defined(_MSC_VER)
#define OX_COMPILER OX_COMPILER_MSVC
#else
#error "Unsupported compiler"
#endif

// ==================== Platform Detection ====================
#if defined(_WIN32) || defined(_WIN64)
#define OX_PLATFORM OX_PLATFORM_WINDOWS
#elif defined(__linux__)
#define OX_PLATFORM OX_PLATFORM_LINUX
#else
#error "Unsupported platform"
#endif

// ==================== Architecture Detection ====================
#if defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__)
#define OX_ARCH OX_ARCH_64
#else
#define OX_ARCH OX_ARCH_32
#endif

// ==================== Build Mode Detection ====================
#if defined(OX_DEBUG)
#define OX_BUILD OX_BUILD_DEBUG
#elif defined(OX_RELEASE)
#define OX_BUILD OX_BUILD_RELEASE
#elif defined(OX_RELDEBUGINFO)
#define OX_BUILD OX_BUILD_RELDEBUGINFO
#elif defined(OX_PRODUCT)
#define OX_BUILD OX_BUILD_PRODUCT
#endif

// ==================== Endiannes Detection ====================
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define OX_ENDIAN OX_ENDIAN_BIG
#else
#define OX_ENDIAN OX_ENDIAN_LITTLE
#endif

#endif // OMNIX_DETECT_H
