//
// Created by cakilgan on 3/16/26.
//

#ifndef OMNIX_HEAP_ALLOCATOR_H
#define OMNIX_HEAP_ALLOCATOR_H
#include <cstring>
#include <omnix/platform/memory.h>
#include <omnix/platform/util.h>

namespace ox {
    struct heap_allocator {
    private:
        void coalesce(){
            block* b = _free_list;
            while (b && b->next) {
                u8* expected = reinterpret_cast<u8*>(b) + sizeof(block) + b->size;
                if (reinterpret_cast<u8*>(b->next) == expected) {
                    b->size += sizeof(block) + b->next->size;
                    b->next = b->next->next;
                } else {
                    b = b->next;
                }
            }
        }
        struct block {
            usize size;
            block* next;
        };

        memory  _mem;
        block*  _free_list;
    public:


        explicit heap_allocator(memory get_memory):_mem(ox::move(get_memory)) {
            OX_ASSERT(_mem.size().ct>sizeof(block));
            _free_list = new (_mem.data()) block();
            _free_list->next = nullptr;
            _free_list->size = _mem.size().ct-sizeof(block);
        }

        vptr alloc(bytes size) {
            block* prev = nullptr;

            for (block* b = _free_list; b != nullptr; prev = b, b = b->next) {
                const usize aligned_size = align(size.ct,16);
                if (b->size >= size.ct) {
                    if (b->size >= aligned_size + sizeof(block)) {

                        const auto new_block = reinterpret_cast<block*>(
                            reinterpret_cast<char*>(b) + sizeof(block) + aligned_size
                        );

                        new_block->size = b->size - aligned_size - sizeof(block);
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
            if (!ptr) return memory::null_parameter;

            const auto b = reinterpret_cast<block *>(static_cast<u8*>(ptr)-sizeof(block));
            block* prev = nullptr;
            block* curr = _free_list;
            while (curr && curr < b) {
                prev = curr;
                curr = curr->next;
            }

            b->next = curr;
            if (prev) prev->next = b;
            else _free_list = b;

            return ok;
        }

        vptr grow(vptr ptr, bytes old_size, bytes new_size) {
            OX_ASSERT(ptr);
            if (!ptr) return nullptr;

            if (const auto new_alloc = alloc(new_size)) {
                memcpy(new_alloc, ptr, old_size);
                this->free(ptr);
                return new_alloc;
            }

            auto* in_place = reinterpret_cast<block *>(static_cast<u8 *>(ptr) - sizeof(block));
            block* b = _free_list;
            block* prev = nullptr;
            while (b) {
                if (reinterpret_cast<char*>(b) == static_cast<char*>(ptr) + old_size.ct)
                    break;
                prev = b;
                b = b->next;
            }

            if (b!=nullptr) {
                if (prev)
                    prev->next = b->next;
                else
                    _free_list = b->next;

                in_place->size = new_size.ct;
                return ptr;
            }

            return nullptr;
        }
    };
}
#endif //OMNIX_HEAP_ALLOCATOR_H