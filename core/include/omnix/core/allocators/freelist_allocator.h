//
// Created by cakilgan on 3/16/26.
//

#ifndef OMNIX_FREELIST_ALLOCATOR_H
#define OMNIX_FREELIST_ALLOCATOR_H
#include <cstring>
#include <omnix/platform/memory.h>
#include <omnix/platform/util.h>

namespace ox {

    // this is NOT fast, but it works.
    struct freelist_allocator {
    private:
        void coalesce(){
            block* b = _free_list;
            while (b && b->next) {
                byte* expected = reinterpret_cast<byte *>(b) + (size_of<block>() + b->size);
                if (reinterpret_cast<byte *>(b->next) == expected) {
                    b->size += size_of<block>() + b->next->size;
                    b->next = b->next->next;
                } else {
                    b = b->next;
                }
            }
        }
        struct block {
            bytes size;
            block* next;
        };

        memory  _mem;
        block*  _free_list{nullptr};
    public:


        explicit freelist_allocator(memory get_memory):_mem(ox::move(get_memory)) {
            OX_ASSERT(_mem.size()>size_of<block>());
            _free_list = new (_mem.data()) block();
            _free_list->next = nullptr;
            _free_list->size = _mem.size()-size_of<block>();
        }
        explicit freelist_allocator() = default;

        vptr alloc(bytes size) {
            block* prev = nullptr;

            for (block* b = _free_list; b != nullptr; prev = b, b = b->next) {
                const auto aligned_size = align(size,bytes(16));
                if (b->size >= aligned_size) {
                    if (b->size >= aligned_size + size_of<block>()) {

                        const auto new_block = reinterpret_cast<block*>(
                            reinterpret_cast<byte*>(b) + (size_of<block>() + aligned_size)
                        );

                        new_block->size = b->size - aligned_size - size_of<block>();
                        new_block->next = b->next;


                        if (prev)
                            prev->next = new_block;
                        else
                            _free_list = new_block;

                    } else {

                        if (prev)
                            prev->next = b->next;
                        else
                            _free_list = b->next;
                    }

                    b->size = aligned_size;

                    return (vptr)(b + 1);
                }
            }
            return nullptr;
        }

        result_t  free(vptr ptr) {
            OX_ASSERT(ptr);
            if (!ptr) return results::err::invalid_parameter;

            if (static_cast<byte*>(ptr) < static_cast<byte *>(_mem.data()) ||
                static_cast<byte*>(ptr) >= static_cast<byte *>(_mem.data()) + _mem.size()) return results::err::invalid_parameter;

            const auto b = reinterpret_cast<block *>(static_cast<byte*>(ptr) - size_of<block>());
            block* prev = nullptr;
            block* curr = _free_list;
            while (curr && curr < b) {
                prev = curr;
                curr = curr->next;
            }

            b->next = curr;
            if (prev) prev->next = b;
            else _free_list = b;

            coalesce();
            return ok;
        }

        vptr grow(vptr ptr, bytes old_size, bytes new_size) {
            OX_ASSERT(ptr);
            if (!ptr) return nullptr;

            const auto aligned_new = align(new_size, bytes(16));
            const auto aligned_old = align(old_size, bytes(16));

            auto* in_place = reinterpret_cast<block*>(
                static_cast<byte*>(ptr) - size_of<block>()
            );

            block* b = _free_list;
            block* prev = nullptr;

            while (b) {
                if (reinterpret_cast<byte*>(b) ==
                    static_cast<byte*>(ptr) + aligned_old)
                    break;

                prev = b;
                b = b->next;
            }

            if (b) {
                const bytes total = aligned_old + size_of<block>() + b->size;

                if (total >= aligned_new) {
                    if (prev) prev->next = b->next;
                    else _free_list = b->next;

                    const bytes remaining = total - aligned_new;

                    if (remaining >= size_of<block>()) {
                        auto* new_block = reinterpret_cast<block*>(
                            reinterpret_cast<byte*>(in_place) +
                            size_of<block>() + aligned_new
                        );

                        new_block->size = remaining - size_of<block>();
                        block* fp = nullptr;
                        block* fc = _free_list;
                        while (fc && fc < new_block) { fp = fc; fc = fc->next; }
                        new_block->next = fc;
                        if (fp) fp->next = new_block;
                        else _free_list = new_block;
                        coalesce();
                    }

                    in_place->size = aligned_new;
                    return ptr;
                }
            }

            if (auto new_alloc = alloc(new_size)) {
                memcpy(new_alloc, ptr, aligned_old.ct);
                this->free(ptr);
                return new_alloc;
            }

            return nullptr;
        }
    };
}
#endif //OMNIX_FREELIST_ALLOCATOR_H
