//
// Created by cakilgan on 3/3/26.
//
#include <omnix/platform/opaque.h>
#include <omnix/platform/types.h>
#include <utility>

namespace ox {
void swap(opaque &a, opaque &b) noexcept {
    using std::swap;
    swap(a._raw, b._raw);
    swap(a._type, b._type);
    swap(a._deleter, b._deleter);
}
} // namespace ox
