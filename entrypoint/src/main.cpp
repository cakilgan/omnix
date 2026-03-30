#include <iostream>
#include <omnix/omnix.h>

#include "omnix/core/engine_memory_internal.h"

ox::memory ENDLESS_MEMORY;

ox::memory GENERAL;
ox::memory GENERAL_SMALL;
ox::memory GENERAL_MID;
ox::memory GENERAL_BIG;

#define MEMORY_RESULT(x,name)\
    if(!x){std::cout<<"ERROR_CODE: "<<x.err()<<"\n"; OX_CRASH(#x);}\
    name = ::ox::move(x.value());\

int main(int argc, char** argv) {
    auto _endless_memory_result =
        ox::memory::allocate(ox::gigabytes(1000));
    MEMORY_RESULT(_endless_memory_result,ENDLESS_MEMORY);

    auto _general_memory_result =
        ox::memory::slice(ENDLESS_MEMORY,
            ox::loczero,ox::gigabytes(100));
    MEMORY_RESULT(_general_memory_result, GENERAL);

    auto _general_small_memory_result =
    ox::memory::slice(GENERAL,
        ox::loczero,ox::gigabytes(20));
    MEMORY_RESULT(_general_small_memory_result, GENERAL_SMALL);

    auto _general_mid_memory_result =
    ox::memory::slice(GENERAL,
        ox::loczero + ox::gigabytes(20) + ox::bytes(1),ox::gigabytes(40));
    MEMORY_RESULT(_general_mid_memory_result, GENERAL_MID);

    auto _general_big_memory_result =
        ox::memory::slice(GENERAL,
            ox::loczero + ox::gigabytes(60),ox::gigabytes(40));
    MEMORY_RESULT(_general_big_memory_result, GENERAL_BIG);

    static ox::freelist_allocator general_small_alloc{ox::move(GENERAL_SMALL)};
    static ox::freelist_allocator general_mid_alloc{ox::move(GENERAL_MID)};
    static ox::freelist_allocator general_big_alloc{ox::move(GENERAL_BIG)};

    engine::memory::general::small = &general_small_alloc;
    engine::memory::general::medium = &general_mid_alloc;
    engine::memory::general::big = &general_big_alloc;
    
    return 0;
}
