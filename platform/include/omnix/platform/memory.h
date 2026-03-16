//
// Created by cakilgan on 3/7/26.
//

#ifndef OMNIX_MEMORY_H
#define OMNIX_MEMORY_H

#include "defines.h"
#include "types.h"
#include "result.h"
#include "byte.h"

//! do not include <assert.h>
#include "../platform/assert.h"

namespace ox {

    // Align a size value to the given alignment.
    OX_FORCE_CONSTEXPR usize align(const usize size, const usize alignment) {
        if (alignment == 0) return size;

        const usize remainder = size % alignment;
        if (remainder == 0)
            return size;

        return size + (alignment - remainder);
    }

    // Returns the next power of two greater than or equal to x.
    OX_FORCE_CONSTEXPR usize nextpow2(usize x) {
        if (x == 0) return 1;

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
    OX_FORCE_CONSTEXPR bool ispow2(usize x) {
        return x != 0 && (x & (x - 1)) == 0;
    }

    // Align a pointer to the given alignment.
    OX_INLINE vptr align_ptr(cvptr ptr, const usize alignment) {
        auto p = reinterpret_cast<uptr>(ptr);
        p = (p + alignment - 1) & ~(alignment - 1);
        return reinterpret_cast<vptr>(p);
    }

    // Returns ptr + offset with safe casting.
    OX_INLINE vptr offset_ptr(cvptr ptr, const usize offset) {
        return reinterpret_cast<vptr>(reinterpret_cast<uptr>(ptr) + offset);
    }


    struct memory {

        // Error codes
        // range: -1321 ... -1328
        static OX_CAUTO slice_overlap              = result_t{-1321};
        static OX_CAUTO max_slice_overflow         = result_t{-1322};
        static OX_CAUTO no_match_for_unslice       = result_t{-1323};
        static OX_CAUTO already_released           = result_t{-1324};
        static OX_CAUTO non_owner_release          = result_t{-1325};
        static OX_CAUTO allocation_error           = result_t{-1326};
        static OX_CAUTO cannot_find_suitable_memory= result_t{-1327};
        static OX_CAUTO null_parameter             = result_t{-1328};


        // Maximum number of slices a memory object can track.
        static OX_FORCE_CONSTEXPR u8 _max_slices_ = 16;

        // Stores information about a single slice.
        struct slice_record {
            loc   start;
            bytes size;
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
        loc _slice_start = 0;


        // Memory types:
        //
        // parent : created with memory::allocate()
        // child  : created from slicing another memory
        // invalid: memory has been released or is unusable
        enum type {
            parent,
            child,
            invalid
        };


        // Default constructor
        memory() : _Data(nullptr), _Size(0), _Type(invalid) {}


        // Move constructor
        // Memory objects can only be transferred via move.
        memory(memory&& o) noexcept {
            _Data = o._Data;
            _Size = o._Size;
            _Type = o._Type;

            o._Data = nullptr;
            o._Size = 0;
            o._Type = invalid;
        }


        // Disable copy operations
        memory(const memory&) = delete;
        memory& operator=(const memory&) = delete;
        memory& operator=(memory) = delete;


        // Allocate memory of given size
        static result<memory> allocate(bytes size);

        // Release allocated memory
        static result_t release(memory& o);

        // Create a slice from the memory
        // range: [start, start + size)
        static result<memory> slice(memory& o, loc start, bytes size, loc ignore_start = -1);

        // Remove a slice from its parent
        static result_t unslice(memory& parent, memory& child, bool set_zero = false);

        // Grow a slice inside its parent memory
        static result_t grow(memory& parent, memory& child, bytes size);

        /*
         * Finds the first suitable location for a slice of the given size.
         *
         * Typical usage:
         *     slice(big_memory, find(big_memory, 1024), 1024);
         */
        static loc find(memory& mem, bytes size, loc ignore_start = -1);


        ~memory() {
            // If this object owns the memory, release it automatically.
            if (is_owner())
                OX_VERIFY(release(*this) == ok);
        }


        // Getters

        vptr data() const noexcept {
            return _Data;
        }

        bytes size() const noexcept {
            return _Size;
        }

        bool is_owner() const noexcept {
            return _Type == parent;
        }

    private:

        vptr  _Data;
        bytes _Size;
        type  _Type;

        // Internal constructor
        memory(vptr data, const bytes size, const type type)
            : _Data(data), _Size(size), _Type(type) {}
    };

} // namespace ox

#endif // OMNIX_MEMORY_H