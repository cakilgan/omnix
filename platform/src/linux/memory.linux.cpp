//
// Created by cakilgan on 3/7/26.
//
#include <omnix/platform/memory.h>
#include <sys/mman.h>

using namespace ox;

result<memory> memory::alloc(bytes size) noexcept {
    void* ptr = ::mmap(nullptr, size.ct,
                   PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS,
                   -1, 0);
    if (ptr == MAP_FAILED) {
        return err;
    }else {
        return memory{ptr,size};
    }
}

void memory::free() noexcept {
    if (_Data) {
        ::munmap(_Data,_Size.ct);
        _Data = nullptr;
        _Size = bytes{0};
        _owned = false;
    }
}

