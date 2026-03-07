//
// Created by cakilgan on 3/2/26.
//

#include <omnix/platform/time.h>
namespace ox {
    using namespace ox::clocks;
    time monotonic::now() {
        return {0};
    }
    time steady::now() {
        return {0};
    }
    time wall::now() {
        return {0};
    }
    time process_relative::now() {
        return {0};
    }
    time thread_relative::now() {
        return {0};
    }
    i32 sleep(const time& dur) {
        return -1;
    }
}