#ifndef OMNIX_ASSERT_H
#define OMNIX_ASSERT_H

#include "types.h"

namespace ox {

    OX_USING(
        assert_handler,
        void(*)(cstr,cstr,i32,cstr)
    );

    // @GLOBAL
    extern  assert_handler g_assert_handler;
} // namespace ox


#if OX_IS(BUILD,DEBUG)

#define OX_ASSERT(expr) \
((expr) ? (void)0 : ::ox::g_assert_handler(#expr, __FILE__, __LINE__, nullptr))
#define OX_ASSERT_MSG(expr, msg) \
((expr) ? (void)0 : ::ox::g_assert_handler(#expr, __FILE__, __LINE__, msg))

#define OX_VERIFY(expr) \
((expr) ? (void)0 : ::ox::g_assert_handler(#expr, __FILE__, __LINE__, nullptr))
#define OX_VERIFY_MSG(expr, msg) \
((expr) ? (void)0 : ::ox::g_assert_handler(#expr, __FILE__, __LINE__, msg))

#else
#define OX_ASSERT(expr) ((void)0)
#define OX_ASSERT_MSG(expr,msg) ((void)0)

#define OX_VERIFY(expr) ((void)(expr))
#define OX_VERIFY_MSG(expr, msg) ((void)(expr))
#endif


#define OX_UNREACHABLE() \
do { OX_ASSERT_MSG(false, "Unreachable code executed"); __builtin_unreachable(); } while(0)

#endif // OMNIX_ASSERT_H