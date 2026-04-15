//
// Created by cakilgan on 3/7/26.
//

#ifndef OMNIX_MEMORY_H
#define OMNIX_MEMORY_H

#include "byte.h"
#include "defines.h"
#include "result.h"
#include "types.h"

//! do not include <assert.h>
#include "../platform/assert.h"

namespace ox {

// Align a size value to the given alignment.
OX_FORCE_CONSTEXPR bytes align(const bytes size, const bytes alignment) {
    if (alignment == bytes(0))
        return size;

    const auto remainder = size % alignment;
    if (remainder == bytes(0))
        return size;

    return size + (alignment - remainder);
}

// Returns the next power of two greater than or equal to x.
OX_FORCE_CONSTEXPR usize nextpow2(usize x) {
    if (x == 0)
        return 1;

    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    if constexpr (sizeof(usize) > 4)
        x |= x >> 32;

    return x + 1;
}

// Checks whether x is a power of two.
OX_FORCE_CONSTEXPR bool ispow2(usize x) { return x != 0 && (x & (x - 1)) == 0; }

// Align a pointer to the given alignment.
OX_INLINE vptr align_ptr(cvptr ptr, const usize alignment) {
    auto p = reinterpret_cast<uptr>(ptr);
    p = (p + alignment - 1) & ~(alignment - 1);
    return reinterpret_cast<vptr>(p);
}

// Returns ptr + offset with safe casting.
OX_INLINE vptr offset_ptr(vptr ptr, const bytes offset) {
    return static_cast<byte *>(ptr) + offset;
}
OX_INLINE cvptr offset_ptr(cvptr ptr, const bytes offset) {
    return static_cast<const byte *>(ptr) + offset;
}

// always define memory categories in base ox:: namespace.
OX_RESULT_CATEGORY(memory, -1320);

struct memory {
    // Error codes
    // range: -1321 ... -1327
    struct err {
        OX_RESULT(memory, slice_overlap);
        OX_RESULT(memory, max_slice_overflow);
        OX_RESULT(memory, no_match_for_unslice);
        OX_RESULT(memory, already_released);
        OX_RESULT(memory, non_owner_release);
        OX_RESULT(memory, allocation_error);
        OX_RESULT(memory, cannot_find_suitable_memory);
    };

    // Maximum number of slices a memory object can track.
    static OX_FORCE_CONSTEXPR u8 _max_slices_ = 16;

    // Stores information about a single slice.
    struct slice_record {
        loc start{};
        bytes size{};
    };

    // Slice record stack
    slice_record _slices_[_max_slices_]{};

    // Current slice count
    u8 _slice_count_ = 0;

    /*
     * When a new slice is created using memory::slice(),
     * the child's _slice_start is set to the "start" parameter.
     *
     * This value is used internally for bounds checking
     * and general slice calculations inside memory functions.
     */
    loc _slice_start = loczero;

    // Memory types:
    //
    // parent : created with memory::allocate()
    // child  : created from slicing another memory
    // invalid: memory has been released or is unusable
    enum type { parent, child, invalid };

    // Default constructor
    memory() : _Data(nullptr), _Size(0), _Type(invalid) {}

    // Move constructor
    // Memory objects can only be transferred via move.
    memory(memory &&o) noexcept {
        _Data = o._Data;
        _Size = o._Size;
        _Type = o._Type;

        o._Data = nullptr;
        o._Size = bytes(0);
        o._Type = invalid;
    }

    // Disable copy operations
    memory(const memory &) = delete;
    memory &operator=(const memory &) = delete;

    memory &operator=(memory &&o) noexcept {
        if (this == &o)
            return *this;

        if (is_owner())
            OX_VERIFY(release(*this) == ok);

        _Data = o._Data;
        _Size = o._Size;
        _Type = o._Type;

        o._Data = nullptr;
        o._Size = bytes(0);
        o._Type = invalid;

        return *this;
    }

    // Allocate memory of given size
    static result<memory> allocate(bytes size);

    // Release allocated memory
    static result_t release(memory &o);

    // Create a slice from the memory
    // range: [start, start + size)
    static result<memory> slice(memory &o, loc start, bytes size,
                                loc ignore_start = locinvalid);

    // Remove a slice from its parent
    static result_t unslice(memory &parent, memory &child,
                            bool set_zero = false);

    // Grow a slice inside its parent memory
    static result_t grow(memory &parent, memory &child, bytes size);

    /*
     * Finds the first suitable location for a slice of the given size.
     *
     * Typical usage:
     *     slice(big_memory, find(big_memory, 1024), 1024);
     */
    static loc find(memory &mem, bytes size, loc ignore_start = locinvalid);

    ~memory() {
        // If this object owns the memory, release it automatically.
        if (is_owner())
            OX_VERIFY(release(*this) == ok);
    }

    // Getters

    vptr data() const noexcept { return _Data; }

    bytes size() const noexcept { return _Size; }

    bool is_owner() const noexcept { return _Type == parent; }
    bool is_invalid() const noexcept { return _Type == invalid; }

  private:
    vptr _Data;
    bytes _Size;
    type _Type;

    // Internal constructor
    memory(vptr data, const bytes size, const type type)
        : _Data(data), _Size(size), _Type(type) {}
};

} // namespace ox

#endif // OMNIX_MEMORY_H
