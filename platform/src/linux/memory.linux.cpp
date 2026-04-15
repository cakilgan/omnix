//
// Created by cakilgan on 3/7/26.
//
#include "omnix/platform/assert.h"
#include <cstring>
#include <new>
#include <omnix/platform/memory.h>
#include <omnix/platform/util.h>
#include <sys/mman.h>
using namespace ox;

result<memory> memory::allocate(bytes size) {
    OX_ASSERT(size != bytes(0));
    void *ptr = ::mmap(nullptr, size.ct, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (ptr == MAP_FAILED) {
        return err::allocation_error;
    }
    return memory{ptr, size, parent};
}

result_t memory::release(memory &o) {
    OX_ASSERT(o.is_owner() && !o.is_invalid());
    if (!o.is_owner())
        return err::non_owner_release;
    if (o._Data) {
        ::munmap(o._Data, o._Size.ct);
        o._Data = nullptr;
        o._Size = bytes(0);
        o._Type = type::invalid;
        return ok;
    }
    return err::already_released;
}

result<memory> memory::slice(memory &o, const loc start, const bytes size,
                             loc ignore_start) {
    OX_ASSERT(!o.is_invalid());
    OX_ASSERT(start + size >= start); // overflow check
    OX_ASSERT(start + size <= loczero + o._Size);

    if (o._slice_count_ >= _max_slices_) {
        OX_FAIL("max_slice_overflow");
        return err::max_slice_overflow;
    }
    for (u8 i = 0; i < o._slice_count_; i++) {
        const auto slice = o._slices_[i];
        auto end1 = start + size;
        auto end2 = slice.start + slice.size;
        bool overlap = !(end1 <= slice.start || start >= end2);
        if (overlap) {
            if (slice.start == ignore_start)
                continue;
            OX_FAIL("memory slice overlap.");
            return err::slice_overlap;
        }
    }
    memory mem{(offset_ptr(o._Data, bytes(start))), size, child};
    mem._slice_start = start;
    u8 pos = o._slice_count_;
    for (u8 i = 0; i < o._slice_count_; i++) {
        if (o._slices_[i].start > start) {
            pos = i;
            break;
        }
    }
    for (u8 i = o._slice_count_; i > pos; i--)
        o._slices_[i] = o._slices_[i - 1];
    o._slices_[pos] = {start, size};
    o._slice_count_++;
    return mem;
}

result_t memory::unslice(memory &owner, memory &child, bool set_zero) {
    OX_ASSERT(!owner.is_invalid());
    bool found = false;
    for (u8 i = 0; i < owner._slice_count_; i++) {
        if (owner._slices_[i].start == child._slice_start) {
            for (u8 x = i; x < owner._slice_count_ - 1; x++) {
                owner._slices_[x] = owner._slices_[x + 1];
            }
            owner._slice_count_--;
            found = true;
        }
    }
    if (!found) {
        OX_FAIL("child not found in owner.");
        return err::no_match_for_unslice;
    }

    if (set_zero) {
        memset(child.data(), 0, child.size().ct);
    }
    child._Data = nullptr;
    child._Size = bytes(0);
    child._Type = type::invalid;
    return ok;
}

loc memory::find(memory &parent, bytes size, loc ignore_start) {
    OX_ASSERT(size != bytes(0));
    OX_ASSERT(!parent.is_invalid());
    loc cursor = loczero;
    bool _ignore = ignore_start != locinvalid;
    for (u8 i = 0; i < parent._slice_count_; i++) {
        const auto slice = parent._slices_[i];
        if (_ignore && slice.start == ignore_start)
            continue;
        if (bytes_between(slice.start, cursor) >= size)
            return cursor;
        cursor = slice.start + slice.size;
    }
    if (bytes_between((loczero + parent._Size), cursor) >= size)
        return cursor;
    return locinvalid;
}

result_t memory::grow(memory &parent, memory &child, bytes size) {
    OX_ASSERT(!parent.is_invalid());
    OX_ASSERT(!child.is_invalid());
    OX_ASSERT(size != bytes(0));

    auto loc = find(parent, size, child._slice_start);
    if (loc == locinvalid)
        return err::cannot_find_suitable_memory;

    auto mhndl = slice(parent, loc, size, child._slice_start);
    if (!mhndl)
        return mhndl.err();

    auto new_memory = ox::move(mhndl.value());

    vptr old_data = child._Data;
    auto old_size = child._Size;

    memmove(new_memory.data(), old_data, old_size.ct);

    unslice(parent, child, false);

    child._Data = new_memory._Data;
    child._Size = new_memory._Size;
    child._Type = new_memory._Type;
    child._slice_start = new_memory._slice_start;

    new_memory._Data = nullptr;
    new_memory._Type = type::invalid;
    new_memory._Size = bytes(0);

    return ok;
}
