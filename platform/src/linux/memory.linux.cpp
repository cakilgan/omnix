//
// Created by cakilgan on 3/7/26.
//
#include <cstring>
#include <omnix/platform/memory.h>
#include <sys/mman.h>
#include <omnix/platform/util.h>

using namespace ox;


result<memory> memory::allocate(bytes size) {
    void* ptr = ::mmap(nullptr, size,
           PROT_READ | PROT_WRITE,
           MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
           -1, 0);
    if (ptr == MAP_FAILED) {
        return allocation_error;
    }
    return memory{ptr,size,parent};
}

result_t       memory::release (memory& o) {
    OX_ASSERT(o.is_owner());
    if (!o.is_owner()) return non_owner_release;
    if (o._Data) {
        ::munmap(o._Data,o._Size);
        o._Data = nullptr;
        o._Size = 0;
        o._Type = type::invalid;
        return ok;
    }
    return already_released;
}

result<memory> memory::slice(memory &o, const loc start, const bytes size, loc ignore_start) {
    OX_ASSERT(start + size.ct <= o._Size);
    if (o._slice_count_>=_max_slices_) {
        OX_FAIL("max_slice_overflow");
        return max_slice_overflow;
    }
    for (u8 i = 0; i < o._slice_count_; i++) {
        const auto slice = o._slices_[i];

        // i am a moron.
        if (in_btw(start,slice.start,slice.size)||
            in_btw(start+size,slice.start,slice.size)||
            contains(start,start+size,slice.start,slice.size)||
            contains(slice.start,slice.size,start,size)) {
            if (slice.start == ignore_start) continue;
            OX_FAIL("memory slice overlap.");
            return slice_overlap;
        }
    }
    memory mem{(offset_ptr(o._Data,start)),size,child};
    mem._slice_start = start;
    u8 pos = o._slice_count_;
    for (u8 i = 0; i < o._slice_count_; i++) {
        if (o._slices_[i].start > start) {
            pos = i;
            break;
        }
    }
    for (u8 i = o._slice_count_; i > pos; i--)
        o._slices_[i] = o._slices_[i-1];
    o._slices_[pos] = {start, size};
    o._slice_count_++;
    return mem;
}

result_t memory::unslice(memory &owner, memory &child,bool set_zero) {
    bool found = false;
    for (u8 i = 0; i < owner._slice_count_; i++) {
        if (owner._slices_[i].start == child._slice_start) {
            for (u8 x = i; x < owner._slice_count_ - 1; x++) {
                owner._slices_[x] = owner._slices_[x+1];
            }
            owner._slice_count_--;
            found = true;
        }
    }
    if (!found) {
        OX_FAIL("child not found in owner.");
        return no_match_for_unslice;
    }

    if (set_zero) {
        memset(child.data(),0,child.size());
    }
    child._Data = nullptr;
    child._Size = 0;
    child._Type = type::invalid;
    return ok;
}

loc memory::find(memory &parent, bytes size, loc ignore_start) {
    loc cursor = 0;
    bool _ignore = ignore_start!=-1;
    for (u8 i = 0; i < parent._slice_count_; i++) {
        const auto slice = parent._slices_[i];
        if (_ignore && slice.start == ignore_start) continue;
        if (slice.start-cursor >= size) return cursor;
        cursor = slice.start+slice.size;
    }
    if (parent._Size - cursor >= size) return cursor;
    return -1;
}


result_t memory::grow(memory &parent, memory &child, bytes size) {
    auto loc = find(parent,size,child._slice_start);
    if (loc==-1) return cannot_find_suitable_memory;

    auto mhndl = slice(parent,loc,size,child._slice_start);
    if (!mhndl) return mhndl.err();

    auto new_memory = ox::move(mhndl.value());

    void* old_data = child._Data;
    auto old_size = child._Size;

    memmove(new_memory.data(), old_data, old_size);

    unslice(parent,child,false);

    child._Data = new_memory._Data;
    child._Size = new_memory._Size;
    child._Type = new_memory._Type;
    child._slice_start = new_memory._slice_start;

    new_memory._Data = nullptr;
    new_memory._Type = type::invalid;
    new_memory._Size = 0;

    return ok;
}