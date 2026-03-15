#ifndef OMNIX_CONSTANTS_H
#define OMNIX_CONSTANTS_H

#include "defines.h"
#include "types.h"


namespace ox {
    OX_INLINE OX_FORCE_CONSTEXPR i8  i8_min  = i8 (       -128                                );
    OX_INLINE OX_FORCE_CONSTEXPR i8  i8_max  = i8 (        127                                );
    OX_INLINE OX_FORCE_CONSTEXPR u8  u8_max  = u8 (        255u                               );

    OX_INLINE OX_FORCE_CONSTEXPR i16 i16_min = i16(       -32768                              );
    OX_INLINE OX_FORCE_CONSTEXPR i16 i16_max = i16(        32767                              );
    OX_INLINE OX_FORCE_CONSTEXPR u16 u16_max = u16(        65535u                             );

    OX_INLINE OX_FORCE_CONSTEXPR i32 i32_min = i32(       -2147483647 - 1                     );
    OX_INLINE OX_FORCE_CONSTEXPR i32 i32_max = i32(        2147483647                         );
    OX_INLINE OX_FORCE_CONSTEXPR u32 u32_max = u32(        4294967295u                        );

    OX_INLINE OX_FORCE_CONSTEXPR i64 i64_min = i64(       -9'223'372'036'854'775'807LL - 1    );
    OX_INLINE OX_FORCE_CONSTEXPR i64 i64_max = i64(        9'223'372'036'854'775'807LL        );
    OX_INLINE OX_FORCE_CONSTEXPR u64 u64_max = u64(        18'446'744'073'709'551'615ULL      );
}


#endif //OMNIX_CONSTANTS_H
