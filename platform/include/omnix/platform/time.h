//
// Created by cakilgan on 3/2/26.
//

#ifndef OMNIX_TIME_H
#define OMNIX_TIME_H

#include "defines.h"
#include "result.h"
#include "types.h"

namespace ox {
OX_RESULT_CATEGORY(time, -54000);

OX_USING(time_t, i64);
OX_USING(timec_t, f32);

struct time {
    struct err {
        OX_RESULT(time, negative_time);
    };

    constexpr time(const time_t nanoseconds = 0) : ns(nanoseconds) {}
    time_t ns;
    bool operator==(const time &rhs) const { return ns == rhs.ns; }
    bool operator!=(const time &rhs) const { return ns != rhs.ns; }
    bool operator<(const time &rhs) const { return ns < rhs.ns; }
    bool operator<=(const time &rhs) const { return ns <= rhs.ns; }
    bool operator>(const time &rhs) const { return ns > rhs.ns; }
    bool operator>=(const time &rhs) const { return ns >= rhs.ns; }
};

OX_FORCE_CONSTEXPR OX_INLINE time seconds(time_t s) {
    return {static_cast<time_t>(s * 1000000000LL)};
}
OX_FORCE_CONSTEXPR OX_INLINE timec_t seconds(const time &s) {
    return static_cast<timec_t>(s.ns) / 1000000000.0f;
}

OX_FORCE_CONSTEXPR OX_INLINE time milliseconds(const time_t ms) {
    return {static_cast<time_t>(ms * 1000000LL)};
}
OX_FORCE_CONSTEXPR OX_INLINE timec_t milliseconds(const time &ms) {
    return static_cast<timec_t>(ms.ns) / 1000000.0f;
}

OX_FORCE_CONSTEXPR OX_INLINE time microseconds(const time_t us) {
    return {static_cast<time_t>(us * 1000LL)};
}
OX_FORCE_CONSTEXPR OX_INLINE timec_t microseconds(const time &us) {
    return static_cast<timec_t>(us.ns) / 1000.0f;
}

OX_FORCE_CONSTEXPR OX_INLINE time nanoseconds(const time_t ns) { return {ns}; }
OX_FORCE_CONSTEXPR OX_INLINE timec_t nanoseconds(const time &ns) {
    return static_cast<timec_t>(ns.ns);
}

OX_FORCE_CONSTEXPR OX_INLINE time operator-(const time &a, const time &b) {
    return {a.ns - b.ns};
}
OX_FORCE_CONSTEXPR OX_INLINE time operator+(const time &a, const time &b) {
    return {a.ns + b.ns};
}
OX_FORCE_CONSTEXPR OX_INLINE time operator*(const time &a, const i64 scalar) {
    return {a.ns * scalar};
}
OX_FORCE_CONSTEXPR OX_INLINE time operator/(const time &a, const i64 scalar) {
    return {a.ns / scalar};
}
OX_FORCE_CONSTEXPR OX_INLINE time operator*(const i64 scalar, const time &a) {
    return {a.ns * scalar};
}
OX_FORCE_CONSTEXPR OX_INLINE time operator/(const i64 scalar, const time &a) {
    return {scalar / a.ns};
}

namespace clocks {
struct monotonic final {
    monotonic() = delete;
    static time now() noexcept;
};
struct steady final {
    steady() = delete;
    static time now() noexcept;
};
struct wall final {
    wall() = delete;
    static time now() noexcept;
};
struct process_relative final {
    process_relative() = delete;
    static time now() noexcept;
};
struct thread_relative final {
    thread_relative() = delete;
    static time now() noexcept;
};
} // namespace clocks

template <typename T = clocks::monotonic> time now() noexcept {
    return T::now();
}

result<time> sleep(const time &dur);

namespace literals {
constexpr ::ox::time operator""_sec(unsigned long long s) {
    return ox::seconds(s);
}
constexpr ::ox::time operator""_ms(unsigned long long ms) {
    return ox::milliseconds(ms);
}
constexpr ::ox::time operator""_us(unsigned long long us) {
    return ox::microseconds(us);
}
constexpr ::ox::time operator""_ns(unsigned long long ns) {
    return ox::nanoseconds(ns);
}
} // namespace literals
} // namespace ox

#endif // OMNIX_TIME_H
