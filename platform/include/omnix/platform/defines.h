//
// Created by cakilgan on 3/2/26.
//

#ifndef OMNIX_DEFINES_H
#define OMNIX_DEFINES_H

#include "macros.h"
#include "detect.h"

#define OX_USING(x,...)   using x = __VA_ARGS__
#define OX_TYPEDEF(x,...) typedef __VA_ARGS__ x

#define OX_IS(x,y) OX_##x == OX_##x##_##y

#if OX_IS(BUILD,DEBUG)
#define OX_CONSTEXPR
#else
#define OX_CONSTEXPR constexpr
#endif
#define OX_FORCE_CONSTEXPR constexpr

#if OX_IS(BUILD,DEBUG)
#define OX_CD_INLINE
#else
#define OX_CD_INLINE inline
#endif
#define OX_INLINE inline


#define OX_STATIC_ASSERT(x,y) static_assert(x,y)

#define OX_FORCE_INLINE inline

#define OX_CAUTO OX_FORCE_CONSTEXPR auto
#endif //OMNIX_DEFINES_H