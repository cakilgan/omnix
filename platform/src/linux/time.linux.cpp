//
// Created by cakilgan on 3/2/26.
//

#include <cerrno>
#include <omnix/platform/time.h>
#include <ctime>

#include "omnix/platform/result.h"
#include <new>

namespace ox {
    static ::ox::time_t _query_posix_ns(clockid_t clk_id) {
        struct timespec ts{};
        clock_gettime(clk_id, &ts);
        return (::ox::time_t)ts.tv_sec * 1000000000LL + (time_t)ts.tv_nsec;
    }

    using namespace ox::clocks;

    time monotonic::now() {
        return {_query_posix_ns(CLOCK_MONOTONIC)};
    }
    time steady::now() {
        return {_query_posix_ns(CLOCK_MONOTONIC_RAW)};
    }
    time wall::now() {
        return {_query_posix_ns(CLOCK_REALTIME)};
    }
    time process_relative::now() {
        struct timespec ts{};
        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == 0) {
            return { (time_t)ts.tv_sec * 1000000000LL + (time_t)ts.tv_nsec };
        }
        return { 0 };
    }
    time thread_relative::now() {
        struct timespec ts{};
        if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts) == 0) {
            return { (::ox::time_t)ts.tv_sec * 1000000000LL + (::ox::time_t)ts.tv_nsec };
        }
        return { 0 };
    }

    // sleeps current execution, returns the remaining nanoseconds.
    result<time> sleep(const time& dur) {
        if (dur<=ox::nanoseconds(0)) return time::err::negative_time;
        struct timespec ts{};
        ts.tv_sec = static_cast<::ox::time_t>(dur.ns / 1000000000LL);
        ts.tv_nsec = static_cast<long>(dur.ns % 1000000000LL);

        struct timespec rem{};
        if (nanosleep(&ts, &rem) == -1) {
            //fixme: implement custom error codes.
            return errno;
        }

        return ox::nanoseconds(rem.tv_nsec);
    }
}
