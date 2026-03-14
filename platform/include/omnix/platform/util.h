//
// Created by cakilgan on 3/8/26.
//

#ifndef OMNIX_UTIL_H
#define OMNIX_UTIL_H
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
        return check != nullptr;
    }
}
#endif //OMNIX_UTIL_H