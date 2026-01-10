
#define OXLIB_DEFAULT_CONFIG
#include <oxlib/oxlib.h>
#include <oxlib/result.h>

#include <iostream>






//time implementation.

#include <oxlib/time.h>
constexpr ox::i64 time_factor(ox::time_t t) {
    switch (t) {
    case ox::time_t::seconds:      return 1'000'000'000;
    case ox::time_t::milliseconds: return 1'000'000;
    case ox::time_t::microseconds: return 1'000;
    case ox::time_t::nanoseconds:  return 1;
    }
    return 1; //NOTE: unreachable
}
ox::time convert(ox::time_t to, const ox::time& from) {
    ox::i64 ns = from.value() * time_factor(from.unit());
    return {
        ns / time_factor(to),
        to
    };
}

ox::time ox::to_seconds(const time& from) {
    return convert(time_t::seconds, from);
}
ox::time ox::to_milliseconds(const time& from) {
    return convert(time_t::milliseconds, from);
}
ox::time ox::to_microseconds(const time& from) {
    return convert(time_t::microseconds, from);
}
ox::time ox::to_nanoseconds(const time& from) {
    return convert(time_t::nanoseconds, from);
}

ox::time ox::as_seconds(const time_v& from_literal) {
    return {
        from_literal,
        time_t::seconds
    };
}
ox::time ox::as_milliseconds(const time_v& from_literal) {
    return {
        from_literal,
        time_t::milliseconds
    };
}
ox::time ox::as_microseconds(const time_v& from_literal) {
    return {
        from_literal,
        time_t::microseconds
    };
}
ox::time ox::as_nanoseconds(const time_v& from_literal) {
    return {
        from_literal,
        time_t::nanoseconds
    };
}

inline void cpu_relax() {
#if defined(__x86_64__) || defined(_M_X64)
    _mm_pause();
#elif defined(__aarch64__)
    asm volatile("yield");
#else
    std::this_thread::yield();
#endif
}

#if OX_PLATFORM == OX_PLATFORM_WINDOWS
#include <windows.h>
#include <thread>
ox::time now_ns() {
    static LARGE_INTEGER freq = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
        }();

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return { (counter.QuadPart * 1'000'000'000) / freq.QuadPart,ox::time_t::nanoseconds };
};



ox::time ox::sleep(const time& duration) {
    auto end = now() + duration;
    ox::time spin = ox::as_microseconds(100);

    auto sleep_until = end - spin;
    auto cur = now();

    if (sleep_until > cur) {
        auto delta = sleep_until - cur;
        std::this_thread::sleep_for(
            std::chrono::microseconds(
                ox::to_microseconds(delta).value()
            )
        );
    }

    while (now() < end)
        cpu_relax();

    return duration;
}

#elif OX_PLATFORM == OX_PLATFORM_LINUX
//TODO: implement linux.
ox::time now_ns() {
    return ox::as_seconds(0);
}
#endif

ox::time ox::now() {
    return now_ns();
}

ox::time_t decide_type(const ox::time& a, const ox::time& b) {
    if (a.unit() > b.unit())
        return a.unit();
    return b.unit();
}

ox::time ox::time_cast(const time_t to, const time& from) {
    switch (to)
    {
        case ox::time_t::seconds:
			return to_seconds(from);
		case ox::time_t::milliseconds:
			return to_milliseconds(from);
		case ox::time_t::microseconds:
			return to_microseconds(from);
		case ox::time_t::nanoseconds:
			return to_nanoseconds(from);
    default:
        break;
    }
	return as_seconds(0); //NOTE: unreachable
}



ox::time ox::operator-(const time& s, const time& e) {
    auto type = decide_type(s, e);
	auto _NewS = time_cast(type, s);
	auto _NewE = time_cast(type, e);
    time _return{
        _NewS.value() - _NewE.value(),
        type
    };
    return _return;
}
ox::time ox::operator+(const ox::time& s, const ox::time& e) {
    auto type = decide_type(s, e);
    auto _NewS = time_cast(type, s);
    auto _NewE = time_cast(type, e);
    time _return{
        _NewS.value() + _NewE.value(),
        type
    };
    return _return;
}
bool ox::operator>(const time& s, const time& e) {
    auto type = decide_type(s, e);
    auto _NewS = time_cast(type, s);
    auto _NewE = time_cast(type, e);
	return _NewS.value() > _NewE.value();
}
bool ox::operator<(const time& s, const time& e) {
    auto type = decide_type(s, e);
    auto _NewS = time_cast(type, s);
    auto _NewE = time_cast(type, e);
    return _NewS.value() < _NewE.value();
}

ox::cstr ox::to_string(const time_t t) {
    switch (t)
    {
        case ox::time_t::seconds:
			return "seconds";
		case ox::time_t::milliseconds:
			return "milliseconds";
		case ox::time_t::microseconds:
			return "microseconds";
		case ox::time_t::nanoseconds:
			return "nanoseconds";
    default:
        break;
    }
	return "_ERROR_"; //NOTE: unreachable
}



//lib implementation

#include <oxlib/lib.h>
#if OX_PLATFORM == OX_PLATFORM_WINDOWS
#include <Windows.h>
ox::dynamic_lib ox::load_lib(ox::cstr lib_path) {
    return dynamic_lib(LoadLibraryA(lib_path));
}
ox::result ox::load_symbol(dynamic_lib& lib, ox::cstr Name) {
    auto symbol = GetProcAddress(reinterpret_cast<HMODULE>(lib.get_handle()), Name);
    if (!symbol) {
        return ox::err;
    }
    lib.symbols.push_back({Name,symbol});
    return ox::ok;
}
ox::result ox::free_lib(dynamic_lib& lib){
    auto res = FreeLibrary(reinterpret_cast<HMODULE>(lib.get_handle())) ? ox::ok : ox::err;
    lib.symbols.clear();
    return res;
}
#endif