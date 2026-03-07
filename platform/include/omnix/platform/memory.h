//
// Created by cakilgan on 3/7/26.
//

#ifndef OMNIX_MEMORY_H
#define OMNIX_MEMORY_H

#include "defines.h"
#include "types.h"
#include "result.h"
#include "byte.h"
#include <array>
#include <cstdint>

namespace ox {
    inline constexpr usize MAX_REGIONS = 64;

    struct region {
        std::size_t offset;
        bytes       size;
        bool        used;
        bool        valid;
    };

    struct memory {
        static result<memory> alloc(bytes size) noexcept;
        void free() noexcept;

        vptr  data()        noexcept       { return _Data; }
        bytes size()  const noexcept       { return _Size; }
        bool  valid() const noexcept       { return _Data != nullptr; }
        explicit operator bool() const noexcept { return valid(); }

        memory(vptr ptr, bytes sz, bool owned = true) noexcept
            : _Data(ptr), _Size(sz), _owned(owned)
        {
            if (ptr) {
                _regions[0] = region{ 0, sz, false, true };
            }
        }

        ~memory() noexcept {
            if (_Data && _owned) free();
        }

        memory(memory&& o) noexcept
            : _Data(o._Data), _Size(o._Size),
              _owned(o._owned), _regions(o._regions)
        {
            o._Data    = nullptr;
            o._Size    = bytes{0};
            o._owned   = false;
            o._regions = {};
        }

        memory& operator=(memory&& o) noexcept {
            if (this != &o) {
                if (_Data && _owned) free();
                _Data    = o._Data;
                _Size    = o._Size;
                _owned   = o._owned;
                _regions = o._regions;
                o._Data    = nullptr;
                o._Size    = bytes{0};
                o._owned   = false;
                o._regions = {};
            }
            return *this;
        }

        memory(const memory&)            = delete;
        memory& operator=(const memory&) = delete;

        memory slice(bytes req) noexcept {
            int   best      = -1;
            bytes best_size { static_cast<bytes_t>(-1) };

            for (int i = 0; i < MAX_REGIONS; ++i) {
                auto& r = _regions[i];
                if (!r.valid || r.used)       continue;
                if (r.size < req) continue;
                if (r.size < best_size) {
                    best      = i;
                    best_size = r.size;
                }
            }

            if (best == -1) return { nullptr, bytes{0}, false };

            auto& chosen        = _regions[best];
            const usize off      = chosen.offset;
            const bytes leftover = chosen.size - req;

            if (leftover > 0) {
                int slot = find_free_slot();
                if (slot != -1) {
                    _regions[slot] = region{
                        off + req.ct,
                        bytes{ leftover },
                        false,
                        true
                    };
                }
            }

            chosen.size = req;
            chosen.used = true;

            memory s{ static_cast<char*>(_Data) + off, req, false };
            s._regions[0] = region{ 0, req, false, true };
            return s;
        }

        void release(const memory& s) noexcept {
            if (!s._Data) return;
            const std::size_t off =
                static_cast<char*>(s._Data) - static_cast<char*>(_Data);

            for (int i = 0; i < MAX_REGIONS; ++i) {
                auto& r = _regions[i];
                if (r.valid && r.used && r.offset == off) {
                    r.used = false;
                    coalesce();
                    return;
                }
            }
        }

    private:
        vptr  _Data    = nullptr;
        bytes _Size    = bytes{0};
        bool  _owned   = false;
        std::array<region, MAX_REGIONS> _regions{};

        int find_free_slot() noexcept {
            for (int i = 0; i < MAX_REGIONS; ++i)
                if (!_regions[i].valid) return i;
            return -1;
        }

        void coalesce() noexcept {
            bool merged = true;
            while (merged) {
                merged = false;
                for (int i = 0; i < MAX_REGIONS && !merged; ++i) {
                    auto& a = _regions[i];
                    if (!a.valid || a.used) continue;
                    for (int j = 0; j < MAX_REGIONS && !merged; ++j) {
                        if (i == j) continue;
                        auto& b = _regions[j];
                        if (!b.valid || b.used) continue;
                        if (a.offset + a.size.ct == b.offset) {
                            a.size.ct += b.size.ct;
                            b = {};
                            merged = true;
                        } else if (b.offset + b.size.ct == a.offset) {
                            b.size.ct += a.size.ct;
                            a = {};
                            merged = true;
                        }
                    }
                }
            }
        }
    };
    //NOTE: not using ::ox::bytes cause of simplicity.

    OX_FORCE_CONSTEXPR usize align(usize size, usize alignment) {
        if (alignment == 0) return size;
        usize remainder = size % alignment;
        if (remainder == 0) return size;
        return size + (alignment - remainder);
    }
    OX_FORCE_CONSTEXPR usize nextpow2(usize x) {
        if (x == 0) return 1;
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        if constexpr (sizeof(usize) > 4) x |= x >> 32;
        return x + 1;
    }
    OX_FORCE_CONSTEXPR bool ispow2(usize x) {
        return x != 0 && (x & (x - 1)) == 0;
    }
    OX_INLINE void* align_ptr(void* ptr, usize alignment) {
        auto p = reinterpret_cast<uptr>(ptr);
        p = (p + alignment - 1) & ~(alignment - 1);
        return reinterpret_cast<void*>(p);
    }
    OX_INLINE void* offset_ptr(void* ptr, usize offset) {
        return reinterpret_cast<void*>(reinterpret_cast<uptr>(ptr) + offset);
    }


} // namespace ox

#endif //OMNIX_MEMORY_H