//
// Created by cakilgan on 4/10/26.
//

#include <omnix/platform/date.h>

namespace ox {

    // Unix epoch: 1970-01-01 00:00:00
    // Gregorian calendar helpers

    static constexpr i64 NANOS_PER_SEC      = 1'000'000'000LL;
    static constexpr i64 NANOS_PER_MILLI    = 1'000'000LL;
    static constexpr i64 NANOS_PER_MICRO    = 1'000LL;

    static constexpr i64 SECS_PER_MIN       = 60LL;
    static constexpr i64 SECS_PER_HOUR      = 3'600LL;
    static constexpr i64 SECS_PER_DAY       = 86'400LL;

    // Returns true if the given year is a leap year (Gregorian)
    static constexpr bool is_leap(i32 y) noexcept {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    // Days in each month for normal and leap years
    static constexpr i32 DAYS_IN_MONTH[2][12] = {
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }, // normal
        { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }, // leap
    };

    date_absolute date(const time& from) {
        date_absolute result{};

        const i64 total_ns   = from.ns;

        // ── Sub-second components ──────────────────────────────────────────
        const i64 sub_ns     = total_ns % NANOS_PER_SEC;        // [0, 1e9)
        const i64 total_secs = total_ns / NANOS_PER_SEC;

        result.detail.nanosecond   = static_cast<i32>(sub_ns % NANOS_PER_MICRO);
        result.detail.microsecond  = static_cast<i32>((sub_ns / NANOS_PER_MICRO) % 1000LL);
        result.detail.millisecond  = static_cast<i32>(sub_ns / NANOS_PER_MILLI);
        result.detail.second       = static_cast<i32>(total_secs % SECS_PER_MIN);

        // ── Time-of-day ────────────────────────────────────────────────────
        i64 day_secs = total_secs % SECS_PER_DAY;
        if (day_secs < 0) day_secs += SECS_PER_DAY;            // handle pre-epoch

        result.value.hour   = static_cast<i32>(day_secs / SECS_PER_HOUR);
        result.value.minute = static_cast<i32>((day_secs % SECS_PER_HOUR) / SECS_PER_MIN);

        // ── Date (days since Unix epoch, 1970-01-01) ───────────────────────
        i64 days = total_secs / SECS_PER_DAY;
        if (total_secs < 0 && total_secs % SECS_PER_DAY != 0) days--; // floor div

        // Shift to an era where day 0 = 0000-03-01 (makes leap-year math easier)
        // Algorithm: civil_from_days (Howard Hinnant, public domain)
        days += 719'468LL;

        const i64 era  = (days >= 0 ? days : days - 146'096LL) / 146'097LL;
        const i64 doe  = days - era * 146'097LL;                              // [0, 146096]
        const i64 yoe  = (doe - doe/1460 + doe/36524 - doe/146096) / 365LL;  // [0, 399]
        const i64 y    = yoe + era * 400LL;
        const i64 doy  = doe - (365*yoe + yoe/4 - yoe/100);                  // [0, 365]
        const i64 mp   = (5*doy + 2) / 153LL;                                // [0, 11]
        const i64 d    = doy - (153*mp + 2)/5 + 1;                           // [1, 31]
        const i64 m    = mp < 10 ? mp + 3 : mp - 9;                          // [1, 12]

        result.value.year  = static_cast<i32>(y + (m <= 2 ? 1 : 0));
        result.value.month = static_cast<i32>(m);
        result.value.day   = static_cast<i32>(d);

        return result;
    }

} // namespace ox