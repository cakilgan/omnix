#include <omnix/omnix.h>

ox::memory ENDLESS_MEMORY;
ox::memory GENERAL;

engine::kernel::s_context* engine::kernel::context = nullptr;


ox::freelist_allocator *engine::memory::general::small = nullptr;
ox::freelist_allocator *engine::memory::general::medium = nullptr;
ox::freelist_allocator *engine::memory::general::big = nullptr;

ox::freelist_allocator *engine::memory::kernel::single = nullptr;


engine::runtime::states engine::runtime::state = states::not_started;

#define MEMORY_RESULT(x,name)\
    if(!x){OX_CRASH(#x);}\
    name = ::ox::move(x.value());\

int main(int argc, char** argv) {
    // inf: installing crash handler functions.
    ox::install_crash_handler();

    //warn: temp memory slices
    ox::memory GENERAL_SMALL;
    ox::memory GENERAL_MID;
    ox::memory GENERAL_BIG;
    ox::memory KERNEL_MEMORY;
    ox::memory LOGGER_MEMORY;
    // warn: end temp memory slices

    // inf: it is technically finite, but you don't have 1000 gb ram, do you?
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
        ox::loczero + ox::gigabytes(20),ox::gigabytes(40));
    MEMORY_RESULT(_general_mid_memory_result, GENERAL_MID);

    auto _general_big_memory_result =
        ox::memory::slice(GENERAL,
            ox::loczero + ox::gigabytes(60),ox::gigabytes(40));
    MEMORY_RESULT(_general_big_memory_result, GENERAL_BIG);

    auto _kernel_memory_result =
        ox::memory::slice(ENDLESS_MEMORY,
            ox::loczero + ox::gigabytes(100)
            ,ox::gigabytes(10));
    MEMORY_RESULT(_kernel_memory_result, KERNEL_MEMORY);

    static ox::freelist_allocator general_small_alloc{ox::move(GENERAL_SMALL)};
    static ox::freelist_allocator general_mid_alloc{ox::move(GENERAL_MID)};
    static ox::freelist_allocator general_big_alloc{ox::move(GENERAL_BIG)};

    engine::memory::general::small = &general_small_alloc;
    engine::memory::general::medium = &general_mid_alloc;
    engine::memory::general::big = &general_big_alloc;

    static ox::freelist_allocator kernel_alloc{ox::move(KERNEL_MEMORY)};
    engine::memory::kernel::single = &kernel_alloc;

    static engine::kernel::s_context kernel_context;
    engine::kernel::context = &kernel_context;

    static engine::config CONFIG;
    static engine::args ARGS{argv, argc};


    engine::runtime::state = engine::runtime::states::init;
    if (engine::runtime::boot(CONFIG, ARGS) != ox::ok)
        return EXIT_FAILURE;


    engine::runtime::state = engine::runtime::states::run;
    auto delta_time = ox::seconds(1)/60;
    while (engine::runtime::state == engine::runtime::states::run) {
        auto start = ox::now();
        if (engine::runtime::pump(delta_time) != ox::ok)
            break;
        auto end = ox::now();
        delta_time = end - start;
    }

    engine::runtime::state = engine::runtime::states::shutdown;
    if (engine::runtime::shutdown_context() != ox::ok)
        return EXIT_FAILURE;

    engine::runtime::state = engine::runtime::states::stop;
    return EXIT_SUCCESS;
}