#ifndef OMNIX_STRING_H
#define OMNIX_STRING_H
#include <omnix/platform.h>

#include "allocators/allocator.h"
#include <omnix/core/strview.h>
#include <omnix/core/engine_memory_internal.h>
#include <cstdarg>
#include <cstdio>

namespace ox {

    template<typename T, typename Allocator = allocator>
    struct string_base {
    public:
        static constexpr usize npos = usize(-1);

    private:
        T*         _raw;
        usize      _length;
        Allocator* _allocator;

        // ── Internal helpers ─────────────────────────────────────────────
        static bytes _u_convert(usize count)  { return size_of<T>() * count; }
        static usize _u_revert(bytes size)    { return size / size_of<T>(); }

        void _u_set_null() { _raw[_length] = T(0); }

        void _u_grow(usize exact = u64_max) {
            const usize old_cap = capacity();
            const usize new_cap = (exact != u64_max) ? exact + 1
                                : (old_cap == 0)     ? 4
                                :                      old_cap * 2;

            T* old_raw  = _raw;
            T* new_data = static_cast<T*>(
                _allocator->grow(_raw, _u_convert(old_cap), _u_convert(new_cap))
            );
            OX_ASSERT(new_data);
            if (new_data != old_raw)
                memcpy(new_data, old_raw, _u_convert(_length + 1).ct);
            _raw = new_data;
        }

        bool _u_should_grow_cond(usize len) const { return len > (capacity() - 1); }
        bool _u_should_grow()              const { return _length >= capacity() - 1; }

        static bool _u_is_space(T c) {
            return c == T(' ')  || c == T('\t') ||
                   c == T('\n') || c == T('\r') ||
                   c == T('\f') || c == T('\v');
        }

    public:
        // ── Constructors / Destructor ─────────────────────────────────────
        string_base(Allocator* allocator, usize reserve = 0)
            : _length(0), _allocator(allocator) {
            OX_ASSERT(allocator != nullptr);
            _raw = static_cast<T*>(allocator->alloc(_u_convert(reserve + 1)));
            OX_ASSERT(_raw);
            _raw[0] = T(0);
        }
        string_base(const T* from, usize len, Allocator* allocator = engine::memory::general::small)
            : string_base(allocator, len) {
            set(from, len);
        }
        explicit string_base(const T* from = "", Allocator* allocator = engine::memory::general::small)
            : string_base(from, ox::strlen(from), allocator) {}
        explicit string_base(usize reserve, Allocator* allocator = engine::memory::general::small)
            : string_base(allocator, reserve) {}

        string_base(string_base&& o) noexcept
            : _raw(o._raw), _length(o._length), _allocator(o._allocator) {
            o._raw = nullptr; o._length = 0; o._allocator = nullptr;
        }
        string_base& operator=(string_base&& o) noexcept {
            if (this != &o) {
                if (_raw && _allocator) OX_VERIFY(_allocator->free(_raw) == ok);
                _raw = o._raw; _length = o._length; _allocator = o._allocator;
                o._raw = nullptr; o._length = 0; o._allocator = nullptr;
            }
            return *this;
        }
        string_base(const string_base&)            = delete;
        string_base& operator=(const string_base&) = delete;

        ~string_base() {
            if (_raw && _allocator) OX_VERIFY(_allocator->free(_raw) == ok);
        }

        // ── Core set ─────────────────────────────────────────────────────
        void set(const T* from, usize len) {
            if (_u_should_grow_cond(len))
                _u_grow(len > capacity() * 2 ? len : u64_max);
            memcpy(_raw, from, _u_convert(len).ct);
            _length = len;
            _u_set_null();
        }
        void set(T fill_char) {
            for (usize i = 0; i < _length; ++i) _raw[i] = fill_char;
            _u_set_null();
        }
        void set(const T* from)       { set(from, ox::strlen(from)); }
        void set(string_view<T> from) { set(from.data, from.len); }

        string_base& operator=(const T* from)       { set(from);  return *this; }
        string_base& operator=(string_view<T> from) { set(from);  return *this; }

        // ── Element access ────────────────────────────────────────────────
              T& operator[](usize i)        { OX_ASSERT(i < _length); return _raw[i]; }
        const T& operator[](usize i)  const { OX_ASSERT(i < _length); return _raw[i]; }
              T& front()                    { OX_ASSERT(_length > 0);  return _raw[0]; }
        const T& front()              const { OX_ASSERT(_length > 0);  return _raw[0]; }
              T& back()                     { OX_ASSERT(_length > 0);  return _raw[_length - 1]; }
        const T& back()               const { OX_ASSERT(_length > 0);  return _raw[_length - 1]; }

        // ── Iterators ─────────────────────────────────────────────────────
              T* begin()        { return _raw; }
        const T* begin()  const { return _raw; }
              T* end()          { return _raw + _length; }
        const T* end()    const { return _raw + _length; }
        const T* cbegin() const { return _raw; }
        const T* cend()   const { return _raw + _length; }

        // ── Capacity / state ─────────────────────────────────────────────
        bool   empty()    const { return _length == 0; }
        usize  length()   const { return _length; }
        usize  capacity() const {
            OX_ASSERT(_allocator);
            auto sz = _allocator->size(_raw);
            OX_ASSERT(sz);
            return _u_revert(sz.value());
        }
        bytes length_bytes()   const { return _u_convert(_length); }
        bytes capacity_bytes() const { return _u_convert(capacity()); }

        void reserve(usize n) {
            if (n + 1 > capacity()) _u_grow(n);
        }
        void clear() {
            _length  = 0;
            _raw[0]  = T(0);
        }
        void resize(usize n, T fill = T(0)) {
            if (n > _length) {
                reserve(n);
                for (usize i = _length; i < n; ++i) _raw[i] = fill;
            }
            _length = n;
            _u_set_null();
        }
        // Shrink internal allocation to exactly fit the current content.
        void shrink_to_fit() {
            const usize needed = _length + 1;
            if (capacity() == needed) return;
            T* new_data = static_cast<T*>(
                _allocator->grow(_raw, _u_convert(capacity()), _u_convert(needed))
            );
            OX_ASSERT(new_data);
            if (new_data != _raw)
                memcpy(new_data, _raw, _u_convert(needed).ct);
            _raw = new_data;
        }

        // ── Append ────────────────────────────────────────────────────────
        void push_back(T c) {
            if (_u_should_grow()) _u_grow();
            _raw[_length++] = c;
            _u_set_null();
        }
        void pop_back() {
            OX_ASSERT(_length > 0);
            _raw[--_length] = T(0);
        }
        void append(const T* s, usize len) {
            const usize new_len = _length + len;
            if (_u_should_grow_cond(new_len))
                _u_grow(new_len > capacity() * 2 ? new_len : u64_max);
            memcpy(_raw + _length, s, _u_convert(len).ct);
            _length = new_len;
            _u_set_null();
        }
        void append(const T* s)           { append(s, ox::strlen(s)); }
        void append(string_view<T> sv)    { append(sv.data, sv.len); }
        void append(T c)                  { push_back(c); }
        void append(const string_base& o) { append(o._raw, o._length); }
        // Append N copies of a character.
        void append_n(T c, usize n) {
            const usize new_len = _length + n;
            if (_u_should_grow_cond(new_len))
                _u_grow(new_len > capacity() * 2 ? new_len : u64_max);
            for (usize i = 0; i < n; ++i) _raw[_length + i] = c;
            _length = new_len;
            _u_set_null();
        }

        string_base& operator+=(const T* s)        { append(s);    return *this; }
        string_base& operator+=(string_view<T> sv) { append(sv);   return *this; }
        string_base& operator+=(T c)               { push_back(c); return *this; }
        string_base& operator+=(const string_base& o) { append(o); return *this; }

        // ── Insert ────────────────────────────────────────────────────────
        void insert(usize pos, const T* s, usize len) {
            OX_ASSERT(pos <= _length);
            const usize new_len = _length + len;
            if (_u_should_grow_cond(new_len))
                _u_grow(new_len > capacity() * 2 ? new_len : u64_max);
            memmove(_raw + pos + len, _raw + pos, _u_convert(_length - pos + 1).ct);
            memcpy(_raw + pos, s, _u_convert(len).ct);
            _length = new_len;
        }
        void insert(usize pos, const T* s)       { insert(pos, s, ox::strlen(s)); }
        void insert(usize pos, string_view<T> sv){ insert(pos, sv.data, sv.len); }
        void insert(usize pos, T c)              { insert(pos, &c, 1); }

        // ── Erase ─────────────────────────────────────────────────────────
        // Remove [pos, pos+count).
        void erase(usize pos, usize count = 1) {
            OX_ASSERT(pos < _length);
            if (count > _length - pos) count = _length - pos;
            memmove(_raw + pos, _raw + pos + count,
                    _u_convert(_length - pos - count + 1).ct);
            _length -= count;
        }
        // Remove all occurrences of character c.
        void erase_all(T c) {
            usize write = 0;
            for (usize read = 0; read < _length; ++read)
                if (_raw[read] != c) _raw[write++] = _raw[read];
            _length = write;
            _u_set_null();
        }

        // ── Replace ───────────────────────────────────────────────────────
        // Replace region [pos, pos+len) with s[0..slen).
        void replace(usize pos, usize len, const T* s, usize slen) {
            OX_ASSERT(pos + len <= _length);
            const usize new_len = _length - len + slen;
            if (_u_should_grow_cond(new_len))
                _u_grow(new_len > capacity() * 2 ? new_len : u64_max);
            memmove(_raw + pos + slen, _raw + pos + len,
                    _u_convert(_length - pos - len + 1).ct);
            memcpy(_raw + pos, s, _u_convert(slen).ct);
            _length = new_len;
        }
        void replace(usize pos, usize len, const T* s)        { replace(pos, len, s, ox::strlen(s)); }
        void replace(usize pos, usize len, string_view<T> sv) { replace(pos, len, sv.data, sv.len); }
        // Replace every occurrence of character `from` with `to`.
        void replace_all(T from, T to) {
            for (usize i = 0; i < _length; ++i)
                if (_raw[i] == from) _raw[i] = to;
        }
        // Replace every occurrence of substring `from` with `to`.
        void replace_all(string_view<T> from, string_view<T> to) {
            if (from.len == 0) return;
            usize pos = 0;
            while ((pos = find(from, pos)) != npos) {
                replace(pos, from.len, to.data, to.len);
                pos += to.len;
            }
        }

        // ── Search ────────────────────────────────────────────────────────
        usize find(T c, usize from = 0) const {
            for (usize i = from; i < _length; ++i)
                if (_raw[i] == c) return i;
            return npos;
        }
        usize find(const T* s, usize slen, usize from = 0) const {
            if (slen == 0)        return from;
            if (slen > _length)   return npos;
            const usize limit = _length - slen;
            for (usize i = from; i <= limit; ++i) {
                usize j = 0;
                while (j < slen && _raw[i + j] == s[j]) ++j;
                if (j == slen) return i;
            }
            return npos;
        }
        usize find(const T* s, usize from = 0)        const { return find(s, ox::strlen(s), from); }
        usize find(string_view<T> sv, usize from = 0) const { return find(sv.data, sv.len, from); }

        usize rfind(T c, usize from = npos) const {
            usize i = (from == npos || from >= _length) ? _length : from + 1;
            while (i > 0) { --i; if (_raw[i] == c) return i; }
            return npos;
        }
        usize rfind(const T* s, usize slen) const {
            if (slen == 0 || slen > _length) return npos;
            for (usize i = _length - slen + 1; i > 0; --i) {
                usize j = 0;
                while (j < slen && _raw[i - 1 + j] == s[j]) ++j;
                if (j == slen) return i - 1;
            }
            return npos;
        }
        usize rfind(const T* s)        const { return rfind(s, ox::strlen(s)); }
        usize rfind(string_view<T> sv) const { return rfind(sv.data, sv.len); }

        bool contains(T c)               const { return find(c)  != npos; }
        bool contains(const T* s)        const { return find(s)  != npos; }
        bool contains(string_view<T> sv) const { return find(sv) != npos; }

        bool starts_with(T c)               const { return _length > 0 && _raw[0] == c; }
        bool starts_with(string_view<T> sv) const {
            if (sv.len > _length) return false;
            for (usize i = 0; i < sv.len; ++i)
                if (_raw[i] != sv.data[i]) return false;
            return true;
        }
        bool starts_with(const T* s) const { return starts_with({s, ox::strlen(s)}); }

        bool ends_with(T c)               const { return _length > 0 && _raw[_length - 1] == c; }
        bool ends_with(string_view<T> sv) const {
            if (sv.len > _length) return false;
            const usize offset = _length - sv.len;
            for (usize i = 0; i < sv.len; ++i)
                if (_raw[offset + i] != sv.data[i]) return false;
            return true;
        }
        bool ends_with(const T* s) const { return ends_with({s, ox::strlen(s)}); }

        // Count non-overlapping occurrences of character / substring.
        usize count(T c) const {
            usize n = 0;
            for (usize i = 0; i < _length; ++i)
                if (_raw[i] == c) ++n;
            return n;
        }
        usize count(string_view<T> sv) const {
            if (sv.len == 0) return 0;
            usize n = 0, pos = 0;
            while ((pos = find(sv, pos)) != npos) { ++n; pos += sv.len; }
            return n;
        }
        usize count(const T* s) const { return count({s, ox::strlen(s)}); }

        // ── Comparison ────────────────────────────────────────────────────
        int compare(const T* s, usize slen) const {
            const usize min_len = _length < slen ? _length : slen;
            for (usize i = 0; i < min_len; ++i) {
                if (_raw[i] < s[i]) return -1;
                if (_raw[i] > s[i]) return  1;
            }
            if (_length < slen) return -1;
            if (_length > slen) return  1;
            return 0;
        }
        int compare(const T* s)           const { return compare(s, ox::strlen(s)); }
        int compare(string_view<T> sv)    const { return compare(sv.data, sv.len); }
        int compare(const string_base& o) const { return compare(o._raw, o._length); }

        bool operator==(const T* s)           const { return compare(s)  == 0; }
        bool operator==(string_view<T> sv)    const { return compare(sv) == 0; }
        bool operator==(const string_base& o) const { return compare(o)  == 0; }
        bool operator!=(const T* s)           const { return compare(s)  != 0; }
        bool operator!=(string_view<T> sv)    const { return compare(sv) != 0; }
        bool operator!=(const string_base& o) const { return compare(o)  != 0; }
        bool operator< (const string_base& o) const { return compare(o)  <  0; }
        bool operator> (const string_base& o) const { return compare(o)  >  0; }
        bool operator<=(const string_base& o) const { return compare(o)  <= 0; }
        bool operator>=(const string_base& o) const { return compare(o)  >= 0; }

        // ── Substring / slices ────────────────────────────────────────────
        // Returns a new owning string containing [pos, pos+len).
        string_base substr(usize pos, usize len = npos) const {
            OX_ASSERT(pos <= _length);
            if (len == npos || pos + len > _length) len = _length - pos;
            return string_base(_raw + pos, len, _allocator);
        }
        // Non-owning view into [pos, pos+len) — zero-copy.
        string_view<T> subview(usize pos, usize len = npos) const {
            OX_ASSERT(pos <= _length);
            if (len == npos || pos + len > _length) len = _length - pos;
            return string_view<T>(_raw + pos, len);
        }

        // ── Case transformation ───────────────────────────────────────────
        void to_upper_inplace() {
            for (usize i = 0; i < _length; ++i) {
                T c = _raw[i];
                if (c >= T('a') && c <= T('z')) _raw[i] = T(c - T('a') + T('A'));
            }
        }
        void to_lower_inplace() {
            for (usize i = 0; i < _length; ++i) {
                T c = _raw[i];
                if (c >= T('A') && c <= T('Z')) _raw[i] = T(c - T('A') + T('a'));
            }
        }

        // ── Trim ──────────────────────────────────────────────────────────
        void trim_left_inplace() {
            usize i = 0;
            while (i < _length && _u_is_space(_raw[i])) ++i;
            if (i > 0) {
                memmove(_raw, _raw + i, _u_convert(_length - i + 1).ct);
                _length -= i;
            }
        }
        void trim_right_inplace() {
            while (_length > 0 && _u_is_space(_raw[_length - 1])) --_length;
            _u_set_null();
        }
        void trim_inplace() { trim_left_inplace(); trim_right_inplace(); }

        // ── Pad ───────────────────────────────────────────────────────────
        // Pad left so total length becomes `width`. No-op if already >= width.
        void pad_left(usize width, T fill = T(' ')) {
            if (_length >= width) return;
            insert(0, nullptr, 0);                 // ensure capacity (noop trick)
            const usize n = width - _length;
            const usize new_len = _length + n;
            if (_u_should_grow_cond(new_len))
                _u_grow(new_len > capacity() * 2 ? new_len : u64_max);
            memmove(_raw + n, _raw, _u_convert(_length + 1).ct);
            for (usize i = 0; i < n; ++i) _raw[i] = fill;
            _length = new_len;
        }
        // Pad right so total length becomes `width`. No-op if already >= width.
        void pad_right(usize width, T fill = T(' ')) {
            if (_length >= width) return;
            append_n(fill, width - _length);
        }

        // ── Reverse ───────────────────────────────────────────────────────
        void reverse_inplace() {
            usize l = 0, r = _length;
            while (l < r) { --r; T tmp = _raw[l]; _raw[l] = _raw[r]; _raw[r] = tmp; ++l; }
        }

        // ── Hash ──────────────────────────────────────────────────────────
        // FNV-1a 64-bit — fast and good distribution for hash maps.
        u64 hash() const {
            constexpr u64 FNV_OFFSET = 14695981039346656037ULL;
            constexpr u64 FNV_PRIME  = 1099511628211ULL;
            u64 h = FNV_OFFSET;
            const unsigned char* p = reinterpret_cast<const unsigned char*>(_raw);
            const usize byte_len   = _length * sizeof(T);
            for (usize i = 0; i < byte_len; ++i) {
                h ^= p[i];
                h *= FNV_PRIME;
            }
            return h;
        }

        // ── sprintf-style format (char strings only) ──────────────────────
        // Overwrites the string with a formatted result.
        void format_v(const char* fmt, va_list args) {
            va_list args2;
            va_copy(args2, args);
            int needed = vsnprintf(nullptr, 0, fmt, args2);
            va_end(args2);
            if (needed < 0) return;
            reserve(static_cast<usize>(needed));
            vsnprintf(reinterpret_cast<char*>(_raw), static_cast<usize>(needed) + 1, fmt, args);
            _length = static_cast<usize>(needed);
        }
        void format(const char* fmt, ...) {
            va_list args; va_start(args, fmt);
            format_v(fmt, args);
            va_end(args);
        }
        // Appends a formatted string to the current content.
        void format_append(const char* fmt, ...) {
            va_list args, args2;
            va_start(args, fmt);
            va_copy(args2, args);
            int needed = vsnprintf(nullptr, 0, fmt, args2);
            va_end(args2);
            if (needed < 0) { va_end(args); return; }
            const usize new_len = _length + static_cast<usize>(needed);
            reserve(new_len);
            vsnprintf(reinterpret_cast<char*>(_raw) + _length,
                      static_cast<usize>(needed) + 1, fmt, args);
            va_end(args);
            _length = new_len;
        }

        // ── Views / raw access ────────────────────────────────────────────
        string_view<T>       view()       { return string_view<T>(_raw, _length); }
        string_view<T>       view() const { return string_view<T>(_raw, _length); }
        const T*             c_str() const { return _raw; }
              T*             data()        { return _raw; }
        const T*             data()  const { return _raw; }
        Allocator*           get_allocator() const { return _allocator; }
    };

    using str  = string_base<char>;
    using wstr = string_base<wchar_t>;

    // ── Free functions ────────────────────────────────────────────────────────

    // Non-mutating copies of case/trim — allocate via provided allocator.
    template<typename T, typename A>
    string_base<T,A> to_upper(string_view<T> sv, A* allocator = engine::memory::general::small) {
        string_base<T,A> r(sv.data, sv.len, allocator);
        r.to_upper_inplace();
        return r;
    }
    template<typename T, typename A>
    string_base<T,A> to_lower(string_view<T> sv, A* allocator = engine::memory::general::small) {
        string_base<T,A> r(sv.data, sv.len, allocator);
        r.to_lower_inplace();
        return r;
    }
    template<typename T, typename A>
    string_base<T,A> trimmed(string_view<T> sv, A* allocator = engine::memory::general::small) {
        string_base<T,A> r(sv.data, sv.len, allocator);
        r.trim_inplace();
        return r;
    }

    // Concatenate two views into a new string.
    template<typename T, typename A = allocator>
    string_base<T,A> concat(string_view<T> a, string_view<T> b,
                             A* allocator = engine::memory::general::small) {
        string_base<T,A> r(allocator, a.len + b.len);
        r.append(a);
        r.append(b);
        return r;
    }

    // sprintf into a fresh string.
    inline str str_format(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        str r(64u); // başlangıç tamponu — büyük çoğunluk için tek alloc yeterli
        r.format_v(fmt, args);
        va_end(args);
        return r;
    }

} // namespace ox
#endif // OMNIX_STRING_H