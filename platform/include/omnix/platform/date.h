//
// Created by cakilgan on 4/10/26.
//

#ifndef OMNIX_DATE_H
#define OMNIX_DATE_H
#include "defines.h"
#include "types.h"
#include "../platform/time.h"

namespace ox {
    OX_USING(date_t,i32);
    struct date_default {
        date_t year;
        date_t month;
        date_t day;
        date_t hour;
        date_t minute;
    };
    struct date_detail {
        date_t second;
        date_t millisecond;
        date_t microsecond;
        date_t nanosecond;
    };
    struct date_absolute {
        date_default value{};
        date_detail  detail{};
    };

    date_absolute date(const time& from);
}
#endif //OMNIX_DATE_H