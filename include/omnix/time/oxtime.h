#ifndef _OX_TIME_H_
#define _OX_TIME_H_
#include <omnix/oxconfig.h>
#include <omnix/oxmacros.h>
#include <omnix/oxdetects.h>
#include <omnix/oxclear.h>
#include <omnix/oxtypes.h>

namespace OX_NAMESPACE{
    OX_TYPEDEF(ox::i64,time_v);

    enum class time_t:i8{
        seconds,
        milliseconds,
        microseconds,
        nanoseconds,
    };

#if OX_CXX_VERSION>=20
using enum time_t;
#endif

    struct time{
        private:
        time_v _Value;
        time_t _Type;
        public:
        time(time_v Value,time_t Type):_Value(Value),_Type(Type){}
        time_v value() const{return _Value;}
        time_t unit() const{return _Type;}
        time_v& value_ref() {return _Value;};
    };

    time to_seconds(const time& from);
    time to_milliseconds(const time& from);
    time to_microseconds(const time& from);
    time to_nanoseconds(const time& from);

    time as_seconds(const time_v& from_literal);
    time as_milliseconds(const time_v& from_literal);
    time as_microseconds(const time_v& from_literal);
    time as_nanoseconds(const time_v& from_literal);
    
    time now();

    time operator-(const time& s,const time& e);


    //SUS: using __rdtsc raw.
    inline time_v cpu(){
        return __rdtsc();
    };
}

#endif