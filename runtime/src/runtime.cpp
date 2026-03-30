
#include <omnix/runtime.h>
#include <omnix/platform/crash.h>

ox::result_t engine::runtime::boot(const engine::config &config, const engine::args &args){
    return ox::ok;
}

ox::i32 engine::runtime::normalize(ox::result_t boot_result){
    if(boot_result != ox::ok){
        std::cout<<"ERROR CODE: "<<boot_result<<"\n";
        OX_CRASH("boot returned something unusal!");
    }
    return 0;
}
