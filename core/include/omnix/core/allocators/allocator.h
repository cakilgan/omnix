//
// Created by cakilgan on 3/30/26.
//

#ifndef OMNIX_ALLOCATOR_H
#define OMNIX_ALLOCATOR_H
#include <omnix/platform/types.h>
#include <omnix/platform/byte.h>
#include <omnix/platform/result.h>
namespace ox {
    struct allocator {
    protected:
        memory  _mem;
    public:
        explicit allocator(memory mem):_mem(ox::move(mem)){}
        virtual ~allocator() = default;
        virtual vptr alloc(bytes size) = 0;
        virtual result_t  free(vptr ptr) = 0;
        virtual vptr grow(vptr ptr, bytes old_size, bytes new_size) = 0;
        virtual result<bytes> size(vptr ptr) const = 0;
    };
}
#endif //OMNIX_ALLOCATOR_H