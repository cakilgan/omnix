#include <omnix/omnix.h>
#include <thread>

ox::memory ENDLESS_MEMORY;
ox::memory GENERAL;

engine::kernel::s_context* engine::kernel::context = nullptr;


ox::freelist_allocator *engine::memory::general::small = nullptr;
ox::freelist_allocator *engine::memory::general::medium = nullptr;
ox::freelist_allocator *engine::memory::general::big = nullptr;

ox::freelist_allocator *engine::memory::kernel::single = nullptr;


engine::runtime::states engine::runtime::state = states::not_started;

engine::logger::log_queue LOG_QUEUE{};
engine::logger::log_queue<> *engine::logger::queue = nullptr;

#define MEMORY_RESULT(x,name)\
    if(!x){OX_CRASH(#x);}\
    name = ::ox::move(x.value()); dbg("%s = ok",#x)\

int main(int argc, char** argv) {
    engine::logger::queue = &LOG_QUEUE;

    auto LOG_WORKER = std::thread(  []{
        engine::logger::log_event ev{};
        while (LOG_QUEUE.pop(ev)) {
            process(ev);
        }
    });
    lifecycle("Hello World!");
    lifecycle("starting OmniX engine...");


    dbg("installing crash handler");
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
    dbg("creating general memory allocators");

    static ox::freelist_allocator kernel_alloc{ox::move(KERNEL_MEMORY)};
    engine::memory::kernel::single = &kernel_alloc;
    dbg("creating kernel memory allocators");

    static engine::kernel::s_context kernel_context;
    engine::kernel::context = &kernel_context;
    dbg("creating kernel context...");

    static engine::config CONFIG;
    static engine::args ARGS{argv, argc};

    engine::runtime::state = engine::runtime::states::init;
    lifecycle("set state to init");
    lifecycle("calling runtime::boot()");
    if (engine::runtime::boot(CONFIG, ARGS) != ox::ok)
        return EXIT_FAILURE;
    lifecycle("runtime::boot() is ok");

    engine::runtime::state = engine::runtime::states::run;
    lifecycle("set state to run");
    auto delta_time = ox::seconds(1)/60;
    dbg("first frame fixed to 1/60");
    while (engine::runtime::state == engine::runtime::states::run) {
        auto start = ox::now();
        if (engine::runtime::pump(delta_time) != ox::ok)
            break;
        auto end = ox::now();
        delta_time = end - start;
    }
    lifecycle("run state is finished");

    engine::runtime::state = engine::runtime::states::shutdown;
    lifecycle("set state to shutdown");
    if (engine::runtime::shutdown_context() != ox::ok)
        return EXIT_FAILURE;


    engine::runtime::state = engine::runtime::states::stop;
    lifecycle("set state to stop");


    lifecycle("destroying logger context");
    lifecycle("stopping OmniX engine...");
    lifecycle("Goodbye World!");
    while (!LOG_QUEUE.empty()) {
        std::this_thread::yield();
    }
    LOG_QUEUE.stop();
    if (LOG_WORKER.joinable()) LOG_WORKER.join();

    return EXIT_SUCCESS;
}