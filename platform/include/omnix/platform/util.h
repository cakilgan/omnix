//
// Created by cakilgan on 3/8/26.
//

#ifndef OMNIX_UTIL_H
#define OMNIX_UTIL_H
#include "result.h"

namespace ox {
    //NOTE: exact copy of std::move but without any external includes
    template<typename _Tp>
    struct remove_reference
    { using type = _Tp; };
    template<typename _Tp>
      struct remove_reference<_Tp&>
    { using type = _Tp; };
    template<typename _Tp>
      struct remove_reference<_Tp&&>
    { using type = _Tp; };
    template<typename _Tp>
        [[__nodiscard__,__gnu__::__always_inline__]]
        constexpr typename remove_reference<_Tp>::type&&
        move(_Tp&& __t) noexcept
    { return static_cast<typename remove_reference<_Tp>::type&&>(__t); }

    OX_INLINE bool safe(vptr check) {
        OX_ASSERT(check != nullptr);
        return check != nullptr;
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


    OX_FORCE_CONSTEXPR bool in_btw(const i64 x, const i64 start, const i64 end) {
        return x >= start && x <= end;
    }
    OX_FORCE_CONSTEXPR bool contains(const i64 x, const i64 y, const i64 start, const i64 end) {
        return x<=start && y>=end;
    }
}
#endif //OMNIX_UTIL_H