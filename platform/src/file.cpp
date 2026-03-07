//
// Created by cakilgan on 3/5/26.
//

#include <omnix/platform/file.h>

namespace ox {

    result<usize> file::write(cstr s) const noexcept {
        if (!s) return result<usize>{ file::invalid };
        usize len = 0;
        while (s[len]) ++len;
        return write(static_cast<cvptr>(s), len);
    }

} // namespace ox