//
// Created by cakilgan on 3/5/26.
//

#include <fcntl.h>
#include <omnix/platform/file.h>
#include <windows.h>

namespace ox {

    result<file> file::open(cstr path, mode m) noexcept {
        return result<file>{ file::invalid };
    }

    file::~file() noexcept {
    }

    bool file::is_open() const noexcept {
        return _handle.is_valid();
    }



    result<usize> file::read(vptr buf, usize len) noexcept {
        return result<usize>{ file::invalid };
    }

    result<usize> file::write(cvptr buf, usize len) noexcept {
        return result<usize>{ file::invalid };
    }

    result_t file::flush() noexcept {
        return file::invalid;
    }


    result<usize> file::seek(i64 offset, seek_mode from) noexcept {
        return result<usize>{ file::invalid };
    }

    result<usize> file::tell() noexcept {
        return seek(0, seek_mode::current);
    }

    result<usize> file::size() noexcept {
        return result<usize>{ file::invalid };
    }

} // namespace ox