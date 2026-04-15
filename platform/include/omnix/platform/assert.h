#ifndef OMNIX_ASSERT_H
#define OMNIX_ASSERT_H

#include "types.h"

namespace ox {
// opaque assert_handler for later use.
OX_USING(assert_handler, void (*)(cstr, cstr, i32, cstr));

// global extern assert_handler value. @GLOBAL
// it is initialized in omnix/platform/src/assert.cpp
extern assert_handler g_assert_handler;
} // namespace ox

#if OX_IS(BUILD, DEBUG)

// classical assertion
#define OX_ASSERT(expr)                                                        \
    ((expr) ? (void)0                                                          \
            : ::ox::g_assert_handler(#expr, __FILE__, __LINE__, nullptr))
// OX_ASSERT with a message
#define OX_ASSERT_MSG(expr, msg)                                               \
    ((expr) ? (void)0 : ::ox::g_assert_handler(#expr, __FILE__, __LINE__, msg))

// it evaluates the expression even in the non-debug modes.
#define OX_VERIFY(expr)                                                        \
    ((expr) ? (void)0                                                          \
            : ::ox::g_assert_handler(#expr, __FILE__, __LINE__, nullptr))
// OX_VERIFY with a message
#define OX_VERIFY_MSG(expr, msg)                                               \
    ((expr) ? (void)0 : ::ox::g_assert_handler(#expr, __FILE__, __LINE__, msg))

#else
#define OX_ASSERT(expr) ((void)0)
#define OX_ASSERT_MSG(expr, msg) ((void)0)
// it evaluates the expression even in the non-debug modes.
#define OX_VERIFY(expr) ((void)(expr))
// OX_VERIFY with a message
#define OX_VERIFY_MSG(expr, msg) ((void)(expr))
#endif

// duh.
#define OX_UNREACHABLE()                                                       \
    do {                                                                       \
        OX_ASSERT_MSG(false, "Unreachable code executed");                     \
        __builtin_unreachable();                                               \
    } while (0)

// it is not an error code.
// if you dont want to write OX_ASSERT_MSG(false,"message")
#define OX_FAIL(msg) OX_ASSERT_MSG(false, msg)

#endif // OMNIX_ASSERT_H
