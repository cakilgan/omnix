//
// Created by cakilgan on 3/8/26.
//

#ifndef OMNIX_UTIL_H
#define OMNIX_UTIL_H
#include "omnix/platform/defines.h"
#include "result.h"
#include "../platform/assert.h"
#include "byte.h"

namespace ox {
    //NOTE: exact copy of std:: type trait , utils.
    template<typename _Tp>
    struct remove_reference
    { using type = _Tp; };
    template<typename _Tp>
      struct remove_reference<_Tp&>
    { using type = _Tp; };
    template<typename _Tp>
      struct remove_reference<_Tp&&>
    { using type = _Tp; };

    template<typename _Tp, _Tp __v>
   struct integral_constant
    {
        static constexpr _Tp value = __v;
        using value_type = _Tp;
        using type = integral_constant<_Tp, __v>;
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };

    template<bool __v>
    using __bool_constant = integral_constant<bool, __v>;
    using true_type =  __bool_constant<true>;
    using false_type = __bool_constant<false>;

    template<typename>
    struct is_lvalue_reference
    : public false_type { };
    template<typename _Tp>
      struct is_lvalue_reference<_Tp&>
      : public true_type { };

    template<typename _Tp>
        [[__nodiscard__,__gnu__::__always_inline__]]
        constexpr typename remove_reference<_Tp>::type&&
        move(_Tp&& __t) noexcept
    { return static_cast<typename remove_reference<_Tp>::type&&>(__t); }

    template<typename _Tp>
    [[__nodiscard__,__gnu__::__always_inline__]]
    constexpr _Tp&&
    forward(typename remove_reference<_Tp>::type& __t) noexcept
    { return static_cast<_Tp&&>(__t); }

    template<typename _Tp>
      [[__nodiscard__,__gnu__::__always_inline__]]
      constexpr _Tp&&
      forward(typename remove_reference<_Tp>::type&& __t) noexcept
    {
        static_assert(!is_lvalue_reference<_Tp>::value,
        "ox::forward must not be used to convert an rvalue to an lvalue");
        return static_cast<_Tp&&>(__t);
    }


    template<typename T>
    T* safe(T* check) {
        OX_CHECK(check!=nullptr);
        return check;
    }

    template<typename T>
    constexpr usize strlen(const T* str) {
        usize count = 0;
        while (str[count] != T(0)) count++;
        return count;
    }


    // DANGEROUS
    // basically just ignores result code and assumes value() is valid and moves it.
    // NOTE: makes assertion for debug mode.
    template<typename T>
    T unchecked(result<T> _result) {
        OX_ASSERT(_result);
        return ox::move(_result.value());
    }

    // DANGEROUS
    // like unchecked(result<T>) but it takes just error codes
    // this is for operations that return result_t rather than result<T>
    // NOTE: makes assertion for debug mode.
    OX_INLINE void unchecked(const result_t _result) {
        OX_ASSERT(_result==ok);
    }

    template<typename T>
    OX_FORCE_CONSTEXPR bytes size_of(const T& obj){
        return bytes(sizeof(T));
    }
    template<typename T>
    OX_FORCE_CONSTEXPR bytes size_of(){
        return bytes{sizeof(T)};
    }


    OX_FORCE_CONSTEXPR bool in_btw(const i64 x, const i64 start, const i64 end) {
        return x >= start && x <= end;
    }
    OX_FORCE_CONSTEXPR bool in_btw(const loc x, const loc start, const loc end) {
        return x >= start && x <= end;
    }

    OX_FORCE_CONSTEXPR bool contains(const i64 x, const i64 y, const i64 start, const i64 end) {
        return x<=start && y>=end;
    }
    OX_FORCE_CONSTEXPR bool contains(const loc x, const loc y, const loc start, const loc end) {
        return x<=start && y>=end;
    }

    OX_FORCE_CONSTEXPR OX_INLINE sloc operator-(loc a, loc b) {
        OX_ASSERT(a.value <= static_cast<u64>(i64_max));
        OX_ASSERT(b.value <= static_cast<u64>(i64_max));
        return sloc{ static_cast<i64>(a.value) - static_cast<i64>(b.value) };
    }
    OX_FORCE_CONSTEXPR OX_INLINE bytes bytes_between(loc a, loc b) {
        return bytes(a.value > b.value ? a.value - b.value : b.value - a.value);
    }

    OX_FORCE_CONSTEXPR OX_INLINE loc operator+(loc l, sloc off) {
        if (off.value >= 0) {
            OX_ASSERT(static_cast<u64>(off.value) <= (u64_max - l.value));
            return loc{ l.value + static_cast<u64>(off.value) };
        } else {
            const u64 neg = static_cast<u64>(-off.value);
            OX_ASSERT(l.value >= neg);
            return loc{ l.value - neg };
        }
    }
    OX_FORCE_CONSTEXPR OX_INLINE loc operator-(loc l, sloc off) {
        return l + sloc{-off.value};
    }

}
#endif //OMNIX_UTIL_H