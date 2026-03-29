//
// Created by cakilgan on 3/5/26.
//

#include <omnix/platform/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstdio>
#include <memory>
#include <sys/mman.h>
#include <omnix/platform/byte.h>
#include <new>

namespace ox {

    // ── lifecycle ───────────────────────────────────

    result<file> file::open(cstr path, u8 m) noexcept {
        if (!path) return result<file>{ results::err::invalid_parameter };

        int flags = 0;
        bool r = m & file::mode::read;
        bool w = (m & file::mode::write) || (m & file::mode::append);  // append = write
        bool a = m & file::mode::append;
        bool c = m & file::mode::create;
        if      ( r && !w) flags = O_RDONLY;
        else if (!r &&  w) flags = O_WRONLY;
        else if ( r &&  w) flags = O_RDWR;
        if (a) flags |= O_APPEND;
        if (c) flags |= O_CREAT;

        int fd = ::open(path, flags, 0644);
        if (fd < 0) {
            switch (errno) {
                case ENOENT: return result<file>{ err::not_found  };
                case EACCES: return result<file>{ err::permission_denied };
                default:     return result<file>{ err::io_error   };
            }
        }

        handle h;
        h.raw = opq::make(new int(fd),true);
        return result<file>{ file(static_cast<handle&&>(h)) };
    }

    file::~file() noexcept {
        if (_handle.is_valid()) {
            if (const int* fd = _handle.raw.as<int>()) ::close(*fd);
        }
    }

    bool file::is_open() const noexcept {
        return _handle.is_valid();
    }

    // ── io ──────────────────────────────────────────

    result<usize> file::read(vptr buf, usize len) const noexcept {
        if (!_handle.is_valid()) return result<usize>{ results::err::invalid_handle };
        int* fd = _handle.raw.as<int>();

        ssize_t n = ::read(*fd, buf, len);
        if (n < 0) return result<usize>{ err::io_error };
        if (n == 0) return result<usize>{ err::eof     };
        return result<usize>{ static_cast<usize>(n) };
    }

    result<usize> file::write(cvptr buf, usize len) const noexcept {
        if (!_handle.is_valid()) return result<usize>{ results::err::invalid_handle };
        int* fd = _handle.raw.as<int>();

        ssize_t n = ::write(*fd, buf, len);
        if (n < 0) return result<usize>{ err::io_error };
        return result<usize>{ static_cast<usize>(n) };
    }

    result_t file::flush() const noexcept {
        if (!_handle.is_valid()) return results::err::invalid_handle;
        int* fd = _handle.raw.as<int>();

        if (::fsync(*fd) < 0) return err::io_error;
        return ok;
    }

    // ── navigation ──────────────────────────────────

    result<usize> file::seek(const i64 offset, const seek_mode from) const noexcept {
        if (!_handle.is_valid()) return result<usize>{ results::err::invalid_handle };
        int* fd = _handle.raw.as<int>();

        int whence = 0;
        switch (from) {
            case seek_mode::start:   whence = SEEK_SET; break;
            case seek_mode::current: whence = SEEK_CUR; break;
            case seek_mode::end:     whence = SEEK_END; break;
        }

        off_t pos = ::lseek(*fd, offset, whence);
        if (pos < 0) return result<usize>{ err::io_error };
        return result<usize>{ static_cast<usize>(pos) };
    }

    result<usize> file::tell() noexcept {
        return seek(0, seek_mode::current);
    }

    result<usize> file::size() const noexcept {
        if (!_handle.is_valid()) return result<usize>{ results::err::invalid_handle };
        int* fd = _handle.raw.as<int>();

        struct stat st{};
        if (::fstat(*fd, &st) < 0) return result<usize>{ err::io_error };
        return result<usize>{ static_cast<usize>(st.st_size) };
    }

    result_t file::hint(access_hint h, i64 offset, i64 len) const noexcept {
        if (!_handle.is_valid()) return results::err::invalid_handle;
        int* fd = _handle.raw.as<int>();

        int advice = 0;
        switch (h) {
            case access_hint::normal:     advice = POSIX_FADV_NORMAL;     break;
            case access_hint::sequential: advice = POSIX_FADV_SEQUENTIAL; break;
            case access_hint::random:     advice = POSIX_FADV_RANDOM;     break;
            case access_hint::will_need:  advice = POSIX_FADV_WILLNEED;   break;
            case access_hint::dont_need:  advice = POSIX_FADV_DONTNEED;   break;
            default: return results::error;
        }

        if (::posix_fadvise(*fd, offset, len, advice) < 0)
            return err::io_error;
        return ok;
    }


    result<file::file_info> file::stat(cstr path) noexcept {
        if (!path) return result<file_info>{ results::err::invalid_parameter };

        struct ::stat st{};
        if (::stat(path, &st) < 0) {
            switch (errno) {
                case ENOENT: return result<file_info>{ err::not_found  };
                case EACCES: return result<file_info>{ err::permission_denied };
                default:     return result<file_info>{ err::io_error   };
            }
        }

        file_info inf{};
        inf.size    = static_cast<usize>(st.st_size);
        inf.is_file = S_ISREG(st.st_mode);
        inf.is_dir  = S_ISDIR(st.st_mode);
        inf.is_link = S_ISLNK(st.st_mode);
        return result<file_info>{ inf };
    }

    result<file::mapped> file::map(usize offset, usize len) const noexcept {
        if (!_handle.is_valid()) return result<mapped>{ results::err::invalid_handle };
        int* fd = _handle.raw.as<int>();

        if (len == 0) {
            struct stat st{};
            if (::fstat(*fd, &st) < 0) return result<mapped>{ err::io_error };
            len = static_cast<usize>(st.st_size);
        }

        void* ptr = ::mmap(nullptr, len, PROT_READ, MAP_PRIVATE, *fd, static_cast<off_t>(offset));
        if (ptr == MAP_FAILED) return result<mapped>{ err::io_error };

        ::madvise(ptr, len, MADV_SEQUENTIAL);

        return result<mapped>{ mapped(ptr, len) };
    }

    file::mapped::~mapped() noexcept {
        if (_data) {
            ::munmap(const_cast<void*>(_data), _size);
            _data = nullptr;
        }
    }

    file::mapped & file::mapped::operator=(mapped &&other) noexcept {
       if (this != &other) {
           if (_data) ::munmap(const_cast<void*>(_data), _size);
           _data = other._data;
           _size = other._size;
           other._data = nullptr;
           other._size = 0;
       }
       return *this;
    }


    result_t file::remove(cstr path) noexcept {
        if (!path) return results::err::invalid_parameter;
        if (::unlink(path) < 0) {
            switch (errno) {
                case ENOENT: return err::not_found;
                case EACCES: return err::permission_denied;
                default:     return err::io_error;
            }
        }
        return ok;
    }

    result_t file::rename(cstr from, cstr to) noexcept {
        if (!from || !to) return results::err::invalid_parameter;

        if (::rename(from, to) == 0) return ok;

        if (errno == EXDEV) {
            const int src = ::open(from, O_RDONLY);
            if (src < 0) return err::not_found;

            const int dst = ::open(to, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (dst < 0) { ::close(src); return err::io_error; }

            const auto buf = std::make_unique<char[]>(65536);
            ssize_t n;
            while ((n = ::read(src, buf.get(), sizeof(buf))) > 0) {
                if (::write(dst, buf.get(), static_cast<usize>(n)) < 0) {
                    ::close(src);
                    ::close(dst);
                    ::unlink(to);
                    return err::io_error;
                }
            }

            ::close(src);
            ::close(dst);

            if (n < 0) {
                ::unlink(to);
                return err::io_error;
            }

            if (::unlink(from) < 0) return err::io_error;
            return ok;
        }

        switch (errno) {
            case ENOENT: return err::not_found;
            case EACCES: return err::permission_denied;
            default:     return err::io_error;
        }
    }

    bool file::exists(cstr path) noexcept{
        return static_cast<bool>(stat(path));
    }
    result<::ox::bytes> file::bytes(cstr path) noexcept{
        const auto fs = stat(path);
        OX_RRESULT(fs);
        return fs.value().size;
    }
    result<bool> file::is_dir(cstr path) noexcept{
        const auto fs = stat(path);
        OX_RRESULT(fs);
        return fs.value().is_dir;
    }
    result<bool> file::is_file(cstr path) noexcept{
        const auto fs = stat(path);
        OX_RRESULT(fs);
        return fs.value().is_file;
    }
    result<bool> file::is_link(cstr path) noexcept{
        const auto fs = stat(path);
        OX_RRESULT(fs);
        return fs.value().is_link;
    }


    //NOTE: for not having to include <new>
    result<usize> buf_heap_writer::write(cvptr buf, usize len) noexcept {
        const char* src     = static_cast<const char*>(buf);
        usize       written = 0;

        while (len > 0) {
            usize avail = _buf_size - _pos;
            usize chunk = len < avail ? len : avail;

            __builtin_memcpy(_buf + _pos, src, chunk);
            _pos    += chunk;
            src     += chunk;
            len     -= chunk;
            written += chunk;

            if (_pos == _buf_size) {
                auto r = flush();
                if (r != ok) return result<usize>{ r };
            }
        }
        return result{ written };
    }
} // namespace ox