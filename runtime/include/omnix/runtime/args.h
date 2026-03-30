#ifndef OMNIX_ARGS_H
#define OMNIX_ARGS_H
#include <omnix/core/vector.h>
#include <omnix/core/strview.h>
#include <omnix/core/engine_memory_internal.h>
#include <iostream>
namespace engine{
    struct args {
    private:
        char** _raw;
        int _element_size;
    public:
        args(char** raw,int element_size)
        :_raw(raw),_element_size(element_size){}
    };
}



#endif // ARGS_H
