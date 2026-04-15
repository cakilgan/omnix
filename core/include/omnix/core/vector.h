//
// Created by cakilgan on 3/16/26.
//

#ifndef OMNIX_VECTOR_H
#define OMNIX_VECTOR_H

#include <omnix/platform/byte.h>
#include <omnix/platform/result.h>
#include <omnix/platform/types.h>
#include <omnix/platform/util.h>

#include "allocators/allocator.h"
#include "engine_memory_internal.h"

#include <initializer_list>

namespace ox {
namespace containers {
template <typename T, typename Allocator = allocator> struct vector {
  private:
    Allocator *_allocator{nullptr};
    T *_data{nullptr};
    usize _capacity{16};
    usize _size{0};

    static bytes _u_convert(usize _to) { return _to * size_of<T>(); }

    result_t _u_grow() {
        const usize new_capacity = (_capacity == 0) ? 4 : _capacity * 2;

        T *new_data = static_cast<T *>(_allocator->grow(
            _data, _u_convert(_capacity), _u_convert(new_capacity)));

        OX_ASSERT(new_data);
        if (!new_data)
            return memory::err::allocation_error;

        for (usize i = 0; i < _size; ++i) {
            new (new_data + i) T(ox::move(_data[i]));
        }

        if (_data) {
            for (usize i = 0; i < _size; ++i) {
                _data[i].~T();
            }
        }

        _data = new_data;
        _capacity = new_capacity;

        return ok;
    }

    bool _u_should_grow() const { return _size == _capacity; }

  public:
    explicit vector(Allocator *allocator, usize reserve = 4)
        : _allocator(allocator), _capacity(reserve) {
        _data = static_cast<T *>(_allocator->alloc(_u_convert(_capacity)));
        OX_ASSERT(_data != nullptr);
    }
    explicit vector(engine::memory::type memory_type = engine::memory::GENERAL,
                    usize reserve = 4)
        : vector(ox::safe(engine::memory::get_allocator<T>(memory_type)),
                 reserve) {}
    explicit vector(std::initializer_list<T> list,
                    engine::memory::type memory_type = engine::memory::GENERAL,
                    usize reserve = 4)
        : vector(memory_type, reserve) {
        for (const auto &elem : list) {
            new (_data + _size++) T(elem);
        }
    }

    vector(const vector &) = delete;
    vector &operator=(const vector &) = delete;

    vector &operator=(vector &&other) noexcept {
        if (this != &other) {
            this->~vector();

            _data = other._data;
            _size = other._size;
            _capacity = other._capacity;
            _allocator = other._allocator;

            other._data = nullptr;
            other._size = 0;
            other._capacity = 0;
        }
        return *this;
    }

    vector(vector &&o) noexcept
        : _allocator(o._allocator), _data(o._data), _capacity(o._capacity),
          _size(o._size) {
        o._allocator = nullptr;
        o._data = nullptr;
        o._capacity = 0;
        o._size = 0;
    }
    ~vector() {
        if (_allocator && _data) {
            clear();
            OX_VERIFY(_allocator->free(_data) == ok);
        }
    }

    void pop_back() {
        OX_ASSERT(_size != 0);
        _data[--_size].~T();
    }

    template <typename... Args> T &emplace_back(Args &&...args) {
        if (_u_should_grow())
            OX_VERIFY(_u_grow() == ok);
        new (_data + _size) T(ox::forward<Args>(args)...);
        return _data[_size++];
    }
    void push_back(const T &x) { emplace_back(x); }
    void push_back(T &&x) { emplace_back(ox::move(x)); }

    T &front() { return _data[0]; }
    T &front() const { return _data[0]; };

    T &back() { return _data[_size - 1]; }
    T &back() const { return _data[_size - 1]; }

    void clear() {
        for (usize i = 0; i < _size; ++i) {
            _data[i].~T();
        }
        _size = 0;
    }

    T &operator[](usize index) {
        OX_ASSERT(index < _size);
        return _data[index];
    }
    const T &at(usize index) const {
        OX_ASSERT(index < _size);
        return _data[index];
    }

    T *begin() { return _data; }
    T *end() { return _data + _size; }

    usize size() const { return _size; }
    usize capacity() const { return _capacity; }
    T *data() const { return _data; }
};
} // namespace containers
using containers::vector;
} // namespace ox

#endif // OMNIX_VECTOR_H
