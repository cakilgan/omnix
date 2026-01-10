#pragma once
#ifndef INCLUDE_GUARD_OMNIXLIB_TIME_HEADER_
#define INCLUDE_GUARD_OMNIXLIB_TIME_HEADER_


#include <oxlib/macros.h>
#include <oxlib/detects.h>
#include <oxlib/types.h>

#ifndef OXLIBCONFIG_SET
OX_INTERNAL_WARNING("config is not set.")
#endif

OX_NAMESPACEDEF {
    OX_TYPEDEF(ox::i64, time_v);

    enum class time_t :i8 {
        seconds,
        milliseconds,
        microseconds,
        nanoseconds,
    };

#if OX_CXX >= OX_CXX_20
    using enum time_t;
#endif
	ox::cstr to_string(const time_t t);

    struct time {
    private:
        time_v _Value;
        time_t _Type;
    public:
        constexpr time(time_v Value, time_t Type) :_Value(Value), _Type(Type) {}

        time_v value() const { return _Value; }
        time_t unit() const { return _Type; }
        time_v& value_ref() { return _Value; };
    };

    time to_seconds(const time& from);
    time to_milliseconds(const time& from);
    time to_microseconds(const time& from);
    time to_nanoseconds(const time& from);

	time time_cast(const time_t to, const time& from);

    time as_seconds(const time_v& from_literal);
    time as_milliseconds(const time_v& from_literal);
    time as_microseconds(const time_v& from_literal);
    time as_nanoseconds(const time_v& from_literal);

    time now();

    time operator-(const time& s, const time& e);
    time operator+(const time& s, const time& e);
	bool operator>(const time& s, const time& e);
    bool operator<(const time& s, const time& e);

    time sleep(const time& duration);

    struct timer {
        time cur = ox::now();
        time elapsed() {
            return ox::now() - cur;
        }
        void reset() {
            cur = ox::now();
        }
    };
}

#endif