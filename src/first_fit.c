#include "allocator.h"
#include <unistd.h>

void *first_fit(int size_of_payload) {
    // align payload to 8 bytes
    if (size_of_payload % 8)
        size_of_payload += 8 - (size_of_payload % 8);

    int total_needed = size_of_payload + OVERHEAD;
    block_header *cur = free_list;

    // first-fit search
    while (cur) {
        if (cur->size >= total_needed) {
            int orig_size = cur->size;
            cur->allocated = 1;
            cur->size      = total_needed;
            
            // Write footer
            block_footer *ftr = (block_footer *)((char *)cur + cur->size - FOOTER_SIZE);
            ftr->size      = cur->size;
            ftr->allocated = cur->allocated;

            int leftover = orig_size - total_needed;
            if ((size_t)leftover < OVERHEAD) {
                // remove cur from free list
                if (cur->prev) cur->prev->next = cur->next;
                if (cur->next) cur->next->prev = cur->prev;
                if (free_list == cur) free_list = cur->next;
            } else {
                // split off a new free block
                block_header *next_free = (block_header *)((char *)cur + total_needed);
                next_free->size      = leftover;
                next_free->allocated = 0;
                next_free->next      = cur->next;
                next_free->prev      = cur->prev;
                if (next_free->prev) next_free->prev->next = next_free;
                if (next_free->next) next_free->next->prev = next_free;
                
                // Write footer for new free block
                block_footer *next_ftr = (block_footer *)((char *)next_free + next_free->size - FOOTER_SIZE);
                next_ftr->size      = next_free->size;
                next_ftr->allocated = next_free->allocated;
                
                free_list = next_free;
            }

            // rewind free_list to head
            while (free_list && free_list->prev)
                free_list = free_list->prev;

            // return payload address
            return (char *)cur + HEADER_SIZE;
        }
        cur = cur->next;
    }
    return NULL;
}