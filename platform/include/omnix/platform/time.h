//
// Created by cakilgan on 3/2/26.
//

#ifndef OMNIX_TIME_H
#define OMNIX_TIME_H

#include "detect.h"
#include "types.h"
#include "defines.h"

namespace ox {
    OX_USING( time_t, i64 );

    struct time{
        time_t ns;
        bool operator==( const time& rhs ) const { return ns == rhs.ns; }
        bool operator!=( const time& rhs ) const { return ns != rhs.ns; }
        bool operator< ( const time& rhs ) const { return ns <  rhs.ns; }
        bool operator<=( const time& rhs ) const { return ns <= rhs.ns; }
        bool operator> ( const time& rhs ) const { return ns >  rhs.ns; }
        bool operator>=( const time& rhs ) const { return ns >= rhs.ns; }
    };

    OX_FORCE_CONSTEXPR OX_INLINE time    seconds         (time_t s)                        { return {s * 1000000000LL};}
    OX_FORCE_CONSTEXPR OX_INLINE time_t  seconds         (const time& s)                   { return      s.ns / 1000000000LL;}

    OX_FORCE_CONSTEXPR OX_INLINE time    milliseconds    (const time_t ms)                 { return {ms * 1000000LL};}
    OX_FORCE_CONSTEXPR OX_INLINE time_t  milliseconds    (const time& ms)                  { return      ms.ns / 1000000LL;}

    OX_FORCE_CONSTEXPR OX_INLINE time    microseconds    (const time_t us)                 { return {us * 1000LL};}
    OX_FORCE_CONSTEXPR OX_INLINE time_t  microseconds    (const time& us)                  { return      us.ns / 1000LL;}

    OX_FORCE_CONSTEXPR OX_INLINE time    nanoseconds     (const time_t ns)                 { return {ns};}
    OX_FORCE_CONSTEXPR OX_INLINE time_t  nanoseconds     (const time& ns)                  { return  ns.ns;}

    OX_FORCE_CONSTEXPR OX_INLINE time    operator-       (const time& a,const time& b)     { return {a.ns - b.ns};   }
    OX_FORCE_CONSTEXPR OX_INLINE time    operator+       (const time& a,const time& b)     { return {a.ns + b.ns};   }
    OX_FORCE_CONSTEXPR OX_INLINE time    operator*       (const time& a,const i64 scalar)  { return {a.ns * scalar}; }
    OX_FORCE_CONSTEXPR OX_INLINE time    operator/       (const time& a,const i64 scalar)  { return {a.ns / scalar}; }
    OX_FORCE_CONSTEXPR OX_INLINE time    operator*       (const i64 scalar,const time& a)  { return {a.ns * scalar}; }
    OX_FORCE_CONSTEXPR OX_INLINE time    operator/       (const i64 scalar,const time& a)  { return {scalar / a.ns}; }

    namespace clocks{
        struct monotonic {
            static time now();
        };
        struct steady {
            static time now();
        };
        struct wall {
            static time now();
        };
        struct process_relative {
            static time now();
        };
        struct thread_relative {
            static time now();
        };
    }

    template<typename T = clocks::monotonic>
    time now() {
        return T::now();
    }

    i32 sleep(const time& dur);
}

#endif //OMNIX_TIME_H