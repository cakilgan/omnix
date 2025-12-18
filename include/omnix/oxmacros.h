#ifndef _OX_MACROS_H_
#define _OX_MACROS_H_

#ifndef _OXCONFIG_DEFINE_
#warning "you should include oxconfig.h or define _OXCONFIG_DEFINE_ before including this file." 
#endif

#define OX_NAMESPACE ox

#define OX_EXPLICIT_TYPEDEF(type,name) typedef type name
#define OX_EXPLICIT_USING(type,name) using name = type

#if OX_CFG_USETYPEDEF
#define OX_TYPEDEF(type,name) OX_EXPLICIT_TYPEDEF(type,name) 
#else
#define OX_TYPEDEF(type,name) OX_EXPLICIT_USING(type,name)
#endif

#define OX_CONSTEXPR constexpr

#define OX_UNDEFINED (-3333333)
#define OX_DISABLED (-134)

#define OX_PLATFORM_WINDOWS 0
#define OX_PLATFORM_LINUX 1
#define OX_PLATFORM_ANDROID 2 

#define OX_TC_MSVC 0
#define OX_TC_CLANG 1
#define OX_TC_GCC 2

#define OX_CXX_VERSION OX_UNDEFINED

#define OX_PLATFORM OX_UNDEFINED
#define OX_TC OX_UNDEFINED

#include <cassert>
#include <cstdio>
#define OX_ASSERT(_Check,_Fmt,...) \
do{if(!(_Check)){printf(_Fmt,__VA_ARGS__);} assert(_Check); } while(0)\


#define OX_FOR(_Id,_Times) for(size_t _Id = 0; _Id<_Times; _Id++)
#define OX_FOR_I(_Times) OX_FOR(i,_Times)

//WARNING: you should include oxdetects.h after this file.
#endif //_OX_MACROS_H_