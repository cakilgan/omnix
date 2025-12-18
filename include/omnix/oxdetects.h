#ifndef OX_DETECTS_H
#define OX_DETECTS_H

#include <omnix/oxmacros.h>

#undef OX_CXX_VERSION


#if defined(_MSC_VER)
    #ifndef _MSVC_LANG
        #define _MSVC_LANG _cplusplus
    #endif
    #define OX_CPP_LANG _MSVC_LANG
#else
    #define OX_CPP_LANG _cplusplus
#endif

#if   OX_CPP_LANG >= 202600L
    #define OX_CXX_VERSION 26
#elif OX_CPP_LANG >= 202302L
    #define OX_CXX_VERSION 23
#elif OX_CPP_LANG >= 202002L
    #define OX_CXX_VERSION 20
#elif OX_CPP_LANG >= 201703L
    #define OX_CXX_VERSION 17
#elif OX_CPP_LANG >= 201402L
    #define OX_CXX_VERSION 14
#elif OX_CPP_LANG >= 201103L
    #define OX_CXX_VERSION 11
#endif

#undef OX_PLATFORM
#undef OX_TC

#if defined(_WIN32)
    #define OX_PLATFORM OX_PLATFORM_WINDOWS
#elif defined(__android__)
    #define OX_PLATFORM OX_PLATFORM_ANDROID
#elif defined(__linux__)
    #define OX_PLATFORM OX_PLATFORM_LINUX
#else
    #error "Unsupported platform"
#endif

#if defined(__clang__)
    #define OX_TC OX_TC_CLANG
    #if defined(_MSC_VER)
        #define OX_CLANG_CL 1
    #endif
#elif defined(_MSC_VER)
    #define OX_TC OX_TC_MSVC
#elif defined(__GNUC__)
    #define OX_TC OX_TC_GCC
#else
    #error "Unsupported compiler"
#endif

#endif