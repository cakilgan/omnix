//
// Created by cakilgan on 3/28/26.
//

#ifndef OMNIX_HASHMAP_H
#define OMNIX_HASHMAP_H
#include "allocators/freelist_allocator.h"

//warn: std
#include <type_traits>

namespace ox {
    namespace containers {
        template<typename T, typename U, typename Allocator = freelist_allocator>
        struct hashmap {
            OX_USING(key  ,  T);
            OX_USING(value,  U);
            enum state {
                empty,occupied,deleted
            };
            struct bucket {
                friend struct hashmap;

                T& key() {
                    return _key;
                }
                U& value() {
                    return _value;
                }

                const T& key() const {
                    return _key;
                }
                const U& value() const {
                    return _value;
                }
            private:
                T _key;
                U _value;
                state _state = empty;
                void destroy() {
                    if (_state == occupied) {
                        _key.~key();
                        _value.~value();
                    }
                }
            };
        private:
            bytes _u_convert_bucket(usize bucket_count) const {
                return bucket_count*size_of<bucket>();
            }

            //warn: not using allocator#grow because it needs to be rehashed.
            void _u_grow() {
                usize old_count = _bucket_count;
                bucket* old = _buckets;

                _bucket_count *= 2;
                _buckets = static_cast<bucket*>(
                    _allocator->alloc(_u_convert_bucket(_bucket_count))
                );

                for (usize i = 0; i < _bucket_count; ++i) {
                    new (&_buckets[i]) bucket{};
                }

                _occupied_buckets = 0;

                for (usize i = 0; i < old_count; ++i) {
                    if (old[i]._state == occupied) {
                        _u_put_no_grow(ox::move(old[i]._key), ox::move(old[i]._value));
                    }
                    if constexpr(!std::is_trivially_destructible_v<U> || !std::is_trivially_destructible_v<T>) {
                        old[i].destroy();
                    }
                }

                _allocator->free(old);
            }
            void _u_if_grow() {
                constexpr f32 MAX_LOAD = 0.7f;
                if (static_cast<f32>(_occupied_buckets)/_bucket_count > MAX_LOAD) _u_grow();
            }

            template<typename K, typename V>
            void _u_put(K&& key,V&& value) {
                _u_if_grow();
                _u_put_no_grow(ox::forward<K>(key),ox::forward<V>(value));
            }
            template<typename K, typename V>
            void _u_put_no_grow(K&& key,V&& value) {

                usize idx = hash</*warn: dont change it ->*/T>{}(key) & (_bucket_count-1);

                for (usize i = 0; i < _bucket_count; ++i) {
                    auto& b = _buckets[(idx + i) & (_bucket_count-1)];

                    if (b._state == occupied && b._key == key) {
                        b._value = ox::forward<V>(value);
                        return;
                    }

                    if (b._state != occupied) {
                        new (&b._key) T(ox::forward<K>(key));
                        new (&b._value) U(ox::forward<V>(value));
                        b._state = occupied;
                        _occupied_buckets++;
                        return;
                    }
                }
            }
            Allocator* _allocator{nullptr};
            usize _bucket_count {0};
            bucket* _buckets {nullptr};
            usize _occupied_buckets {0};
        public:
            explicit hashmap(Allocator* allocator, const usize reserve_bucket = 4)
            :_allocator(allocator){
                _bucket_count = reserve_bucket > 0 ? nextpow2(reserve_bucket) : 4;
                _buckets = static_cast<bucket *>(allocator->alloc(_u_convert_bucket(_bucket_count)));
                for (usize i = 0; i < _bucket_count; ++i)
                    new (&_buckets[i]) bucket{};
            }
            hashmap(const hashmap&) = delete;
            hashmap& operator=(const hashmap&) = delete;

            hashmap(hashmap&& other) noexcept
                : _allocator(other._allocator),
                  _bucket_count(other._bucket_count),
                  _buckets(other._buckets),
                  _occupied_buckets(other._occupied_buckets) {

                other._buckets = nullptr;
                other._bucket_count = 0;
                other._occupied_buckets = 0;
                other._allocator = nullptr;
            }

            hashmap& operator=(hashmap&& other) noexcept {
                if (this != &other) {
                    this->~hashmap();

                    _allocator = other._allocator;
                    _buckets = other._buckets;
                    _bucket_count = other._bucket_count;
                    _occupied_buckets = other._occupied_buckets;

                    other._buckets = nullptr;
                    other._bucket_count = 0;
                    other._occupied_buckets = 0;
                    other._allocator = nullptr;
                }
                return *this;
            }

            ~hashmap() {
                if (_buckets) {
                    for (usize i = 0; i < _bucket_count; ++i) {
                        _buckets[i].destroy();
                    }
                    _allocator->free(_buckets);
                }
            }

            template<typename K, typename V>
            void put(K&& key, V&& value) {
                _u_put(ox::forward<K>(key), ox::forward<V>(value));
            }

            bool get(const T& key, U& out) const {
                const usize idx = hash<T>{}(key) & (_bucket_count-1);
                for (usize i = 0; i < _bucket_count; ++i) {
                    const auto& b = _buckets[(idx + i) & (_bucket_count-1)];
                    if (b._state == empty) {
                        return false;
                    }
                    if (b._state == occupied && b._key == key) {
                        out = b._value;
                        return true;
                    }
                }
                return false;
            }

            bool remove(const T& key) {
                const usize idx = hash<T>{}(key) & (_bucket_count-1);
                for (usize i = 0; i < _bucket_count; ++i) {
                    auto& b = _buckets[(idx + i) & (_bucket_count-1)];
                    if (b._state == empty) {
                        return false;
                    }
                    if (b._state == occupied && b._key == key) {
                        b.destroy();
                        b._state = deleted;
                        _occupied_buckets--;
                        return true;
                    }
                }
                return false;
            }


            struct iterator {
                bucket* _current;
                bucket* _end;

                iterator(bucket* start, bucket* end) : _current(start), _end(end) {
                    while (_current < _end && _current->_state != occupied) ++_current;
                }

                bucket& operator*() const { return *_current; }
                bucket* operator->() const { return _current; }

                iterator& operator++() {
                    if (_current < _end) ++_current;
                    while (_current < _end && _current->_state != occupied) ++_current;
                    return *this;
                }

                bool operator!=(const iterator& other) const { return _current != other._current; }
            };
            struct const_iterator {
                const bucket* _current;
                const bucket* _end;

                const_iterator(const bucket* start, const bucket* end) : _current(start), _end(end) {
                    while (_current < _end && _current->_state != occupied) ++_current;
                }

                const bucket& operator*() const { return *_current; }
                const bucket* operator->() const { return _current; }

                const_iterator& operator++() {
                    if (_current < _end) ++_current;
                    while (_current < _end && _current->_state != occupied) ++_current;
                    return *this;
                }

                bool operator!=(const const_iterator& other) const { return _current != other._current; }
            };

            iterator begin() { return iterator(_buckets, _buckets + _bucket_count); }
            iterator end()   { return iterator(_buckets + _bucket_count, _buckets + _bucket_count); }
            const_iterator begin() const { return const_iterator(_buckets, _buckets + _bucket_count); }
            const_iterator end()   const { return const_iterator(_buckets + _bucket_count, _buckets + _bucket_count); }
        };
    }
    using containers::hashmap;
}
#endif //OMNIX_HASHMAP_H