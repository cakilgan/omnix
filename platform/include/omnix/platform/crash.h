//
// Created by cakilgan on 3/29/26.
//

#ifndef OMNIX_CRASH_H
#define OMNIX_CRASH_H
#include "defines.h"
#if OX_IS(COMPILER,MSVC)
    #define OX_TRAP() __debugbreak()
#elif OX_IS(COMPILER,CLANG) || OX_IS(COMPILER,GCC)
    #define OX_TRAP() __builtin_trap()
#else
    #include <cstdlib>
    #define OX_TRAP() std::abort()
#endif

#define OX_CRASH(msg) ::ox::panic(msg)
#define OX_CHECK(x) \
do { \
if (!(x)) { \
::ox::panic("CHECK FAILED: " #x); \
} \
} while(0)

namespace ox {
    void panic(const char* msg);
    void crash_handler(int sig);
    void install_crash_handler();
}
#endif //OMNIX_CRASH_H