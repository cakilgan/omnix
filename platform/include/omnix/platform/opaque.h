//
// Created by cakilgan on 3/29/26.
//

#ifndef OMNIX_OPAQUE_H
#define OMNIX_OPAQUE_H

#include "crash.h"
#include "types.h"

namespace ox {
namespace types {
template <typename T> struct default_deleter {
    static void destroy(T *del) { delete del; }
};
template <typename T> struct default_deleter_arr {
    static void destroy(T *del) { delete[] del; }
};

struct opaque {
    void *_raw = nullptr;
    type_id _type{type_of<void>()};
    void (*_deleter)(void *) = nullptr;
    bool _owner{false};
    opaque() = default;

    template <typename T, typename deleter = default_deleter<T>>
    static opaque make(T *ptr, bool _owner = false) {
        return opaque{ptr, type_of<T>(),
                      [](vptr p) { deleter::destroy(static_cast<T *>(p)); },
                      _owner};
    }

    ~opaque() { reset(); }

    void reset() noexcept {
        if (_raw && _deleter && _owner) {
            _deleter(_raw);
        }
        _raw = nullptr;
        _deleter = nullptr;
        _type = type_of<void>();
    }

    opaque(opaque &&other) noexcept
        : _raw(other._raw), _type(other._type), _deleter(other._deleter),
          _owner(other._owner) {
        other._raw = nullptr;
        other._deleter = nullptr;
        other._type = type_of<void>();
        other._owner = false;
    }

    opaque &operator=(opaque &&other) noexcept {
        if (this != &other) {
            reset();
            _raw = other._raw;
            _type = other._type;
            _deleter = other._deleter;
            _owner = other._owner;

            other._raw = nullptr;
            other._deleter = nullptr;
            other._type = type_of<void>();
            other._owner = false;
        }
        return *this;
    }

    opaque(const opaque &) = delete;
    opaque &operator=(const opaque &) = delete;

    template <typename T> T *as() const noexcept {
        if (_type != type_of<T>()) {
            OX_CRASH("underlying opaque type is not same with requested type.");
            return nullptr;
        }
        return static_cast<T *>(_raw);
    }

    explicit operator bool() const noexcept { return _raw != nullptr; }

    vptr release() noexcept {
        vptr tmp = _raw;
        _raw = nullptr;
        _deleter = nullptr;
        _type = type_of<void>();
        _owner = false;
        return tmp;
    }

    friend void swap(opaque &a, opaque &b) noexcept;

  private:
    opaque(void *p, const type_id &ti, void (*d)(void *), bool owner)
        : _raw(p), _type(ti), _deleter(d), _owner(owner) {}
};
} // namespace types
} // namespace ox
#endif // OMNIX_OPAQUE_H
