//
// Created by cakilgan on 3/5/26.
//

#ifndef OMNIX_FILE_H
#define OMNIX_FILE_H

#include "byte.h"
#include "opaque.h"
#include "result.h"
#include "types.h"

namespace ox {

// always define memory categories in base ox:: namespace.
OX_RESULT_CATEGORY(file, -100001);

struct file {
  public:
    struct err {
        OX_RESULT(file, not_found);
        OX_RESULT(file, permission_denied);
        OX_RESULT(file, io_error);
        OX_RESULT(file, eof);
        OX_RESULT(file, too_large);
    };

    struct handle {
        opaque raw;
        handle() = default;
        handle(const handle &) = delete;
        handle &operator=(const handle &) = delete;
        handle(handle &&) noexcept = default;
        handle &operator=(handle &&) noexcept = default;
        bool is_valid() const { return static_cast<bool>(raw); }
    };

    struct mode {
        static OX_CAUTO read = u8{0b00000001};
        static OX_CAUTO write = u8{0b00000010};
        static OX_CAUTO append = u8{0b00000100};
        static OX_CAUTO create = u8{0b00001000};
    };

    enum class seek_mode : u8 { start, current, end };

    enum class access_hint : u8 {
        normal,
        sequential,
        random,
        will_need,
        dont_need,
    };

    struct mapped {
      public:
        ~mapped() noexcept;

        mapped(mapped &&other) noexcept
            : _data(other._data), _size(other._size) {
            other._data = nullptr;
            other._size = 0;
        }

        mapped &operator=(mapped &&other) noexcept;
        mapped(const mapped &) = delete;
        mapped &operator=(const mapped &) = delete;

        const void *data() const noexcept { return _data; }
        usize size() const noexcept { return _size; }
        bool valid() const noexcept { return _data != nullptr; }
        const void *operator[](usize offset) const noexcept {
            return static_cast<const basic_byte *>(_data) + offset;
        }

      private:
        friend struct file;
        mapped(const void *data, usize size) : _data(data), _size(size) {}

        const void *_data = nullptr;
        usize _size = 0;
    };

    struct file_info {
        usize size;
        bool is_file;
        bool is_dir;
        bool is_link;
    };
    static result<file_info> stat(cstr path) noexcept;
    static bool exists(cstr path) noexcept;
    static result<::ox::bytes> bytes(cstr path) noexcept;
    static result<bool> is_dir(cstr path) noexcept;
    static result<bool> is_file(cstr path) noexcept;
    static result<bool> is_link(cstr path) noexcept;
    static result_t remove(cstr path) noexcept;
    static result_t rename(cstr from, cstr to) noexcept;

    result<mapped> map(usize offset = 0, usize len = 0) const noexcept;

    static result<file> open(cstr path, u8 m) noexcept;
    ~file() noexcept;

    file(file &&) noexcept = default;
    file &operator=(file &&) noexcept = default;
    file(const file &) = delete;
    file &operator=(const file &) = delete;

    result_t hint(access_hint h, i64 offset = 0, i64 len = 0) const noexcept;

    result<usize> read(vptr buf, usize len) const noexcept;

    result<usize> write(cvptr buf, usize len) const noexcept;
    result<usize> write(cstr s) const noexcept;

    result<usize> seek(i64 offset, seek_mode from) const noexcept;
    result<usize> tell() noexcept;

    result<usize> size() const noexcept;
    bool is_open() const noexcept;
    result_t flush() const noexcept;

  private:
    explicit file(handle h) : _handle(static_cast<handle &&>(h)) {}
    handle _handle;
};

struct buf_heap_writer {
  public:
    explicit buf_heap_writer(file &f, usize size = 1024 * 1024) noexcept
        : _file(&f), _buf(new char[size]), _buf_size(size), _pos(0) {}

    ~buf_heap_writer() noexcept {
        flush();
        delete[] _buf;
    }

    buf_heap_writer(buf_heap_writer &&) noexcept = default;
    buf_heap_writer &operator=(buf_heap_writer &&) noexcept = default;
    buf_heap_writer(const buf_heap_writer &) = delete;
    buf_heap_writer &operator=(const buf_heap_writer &) = delete;

    result<usize> write(cvptr buf, usize len) noexcept;
    result<usize> write(cstr s) noexcept {
        if (!s)
            return result<usize>{results::err::invalid_parameter};
        usize len = 0;
        while (s[len])
            ++len;
        return write(static_cast<cvptr>(s), len);
    }

    result_t flush() noexcept {
        if (_pos == 0)
            return ok;
        auto r = _file->write(static_cast<cvptr>(_buf), _pos);
        if (!r)
            return r.err();
        _pos = 0;
        return ok;
    }

  private:
    file *_file = nullptr;
    dstr _buf = nullptr;
    usize _buf_size = 0;
    usize _pos = 0;
};

template <usize N = 65536> struct buf_stack_writer {
    static_assert(N <= 512 * 1024, "512KB ustu icin buffered_writer kullan");

  public:
    explicit buf_stack_writer(file &f) noexcept : _file(&f), _pos(0) {}

    ~buf_stack_writer() noexcept { flush(); }

    buf_stack_writer(buf_stack_writer &&) noexcept = default;
    buf_stack_writer &operator=(buf_stack_writer &&) noexcept = default;
    buf_stack_writer(const buf_stack_writer &) = delete;
    buf_stack_writer &operator=(const buf_stack_writer &) = delete;

    result<usize> write(cvptr buf, usize len) noexcept {
        const char *src = static_cast<const char *>(buf);
        usize written = 0;

        while (len > 0) {
            usize avail = N - _pos;
            usize chunk = len < avail ? len : avail;

            __builtin_memcpy(_buf + _pos, src, chunk);
            _pos += chunk;
            src += chunk;
            len -= chunk;
            written += chunk;

            if (_pos == N) {
                auto r = flush();
                if (r != ok)
                    return result<usize>{r};
            }
        }
        return result<usize>{written};
    }

    result<usize> write(cstr s) noexcept {
        if (!s)
            return result<usize>{results::err::invalid_parameter};
        usize len = 0;
        while (s[len])
            ++len;
        return write(static_cast<cvptr>(s), len);
    }

    result_t flush() noexcept {
        if (_pos == 0)
            return ok;
        auto r = _file->write(static_cast<cvptr>(_buf), _pos);
        if (!r)
            return r.err();
        _pos = 0;
        return ok;
    }

  private:
    file *_file = nullptr;
    char _buf[N];
    usize _pos = 0;
};

} // namespace ox
#endif // OMNIX_FILE_H
