#include <omnix/time/oxtime.h>

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

ox::time ox::to_seconds(const time &from){
    return convert(time_t::seconds,from);
}
ox::time ox::to_milliseconds(const time &from){
    return convert(time_t::milliseconds,from);
}
ox::time ox::to_microseconds(const time &from){
    return convert(time_t::microseconds,from);
}
ox::time ox::to_nanoseconds(const time &from){
    return convert(time_t::nanoseconds,from);
}

ox::time ox::as_seconds(const time_v &from_literal){
    return {
        from_literal,
        time_t::seconds
    };
}
ox::time ox::as_milliseconds(const time_v &from_literal){
    return {
        from_literal,
        time_t::milliseconds
    };
}
ox::time ox::as_microseconds(const time_v &from_literal){
    return {
        from_literal,
        time_t::microseconds
    };
}
ox::time ox::as_nanoseconds(const time_v &from_literal){
    return {
        from_literal,
        time_t::nanoseconds
    };
}


#if OX_PLATFORM == OX_PLATFORM_WINDOWS
#include <windows.h>
ox::time now_ns(){
    static LARGE_INTEGER freq = []{
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return {(counter.QuadPart * 1'000'000'000) / freq.QuadPart,ox::time_t::nanoseconds};
};
#elif OX_PLATFORM == OX_PLATFORM_LINUX
//TODO: implement linux.
#endif

ox::time ox::now(){
    return now_ns();
}

ox::time ox::operator-(const time& s,const time& e){
    OX_ASSERT(s.unit()==e.unit(), "%s\n","time units should match!");
    time _return{
        s.value()-e.value(),
        s.unit()
    };
    return _return;
}