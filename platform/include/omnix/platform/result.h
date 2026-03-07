//
// Created by cakilgan on 3/5/26.
//

#ifndef OMNIX_RESULT_H
#define OMNIX_RESULT_H
#include "defines.h"
#include "types.h"
namespace ox {
    OX_USING(result_t,i64);

    template<typename T>
struct result {
    public:

        result(const T& v)
            : _has_value(true)
        {
            new(&_storage.value) T(v);
        }

        result(T&& v)
            : _has_value(true)
        {
            new(&_storage.value) T(static_cast<T&&>(v));
        }

        result(result_t err)
            : _has_value(false)
        {
            _storage.error = err;
        }

        result(const result& other)
            : _has_value(other._has_value)
        {
            if (_has_value)
                new(&_storage.value) T(other._storage.value);
            else
                _storage.error = other._storage.error;
        }

        result(result&& other)
 noexcept             : _has_value(other._has_value)
        {
            if (_has_value)
                new(&_storage.value) T(static_cast<T&&>(other._storage.value));
            else
                _storage.error = other._storage.error;
        }

        ~result() {
            if (_has_value)
                _storage.value.~T();
        }

        explicit operator bool() const {
            return _has_value;
        }

        T& value() {
            return _storage.value;
        }

        const T& value() const {
            return _storage.value;
        }

        result_t err() const {
            return _storage.error;
        }

    private:

        union storage {
            T value;
            result_t error{};

            storage() {}
            ~storage() {}
        } _storage;

        bool _has_value;
    };

    namespace results {
        OX_CAUTO ok  = result_t{0};
        OX_CAUTO err = result_t{-1};
    };

    using results::ok;
    using results::err;


}

#define OX_RESULTV(x,...) \
    if(x){\
    __VA_ARGS__\
    }\
    return x.err();\

#endif //OMNIX_RESULT_H