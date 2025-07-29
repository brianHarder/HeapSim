#include "allocator.h"
#include <stddef.h>

// rewrite footer to match header
static inline void write_footer(block_header *hdr) {
    block_footer *ftr = (block_footer *)((char *)hdr + hdr->size - FOOTER_SIZE);
    ftr->size      = hdr->size;
    ftr->allocated = hdr->allocated;
}

void free_block(void *ptr) {
    if (!ptr) return;

    block_header *hdr = (block_header *)((char *)ptr - HEADER_SIZE);
    hdr->allocated = 0;
    write_footer(hdr);

    char *heap_base = (char*)heap_start;
    char *heap_end  = heap_base + heap_size;

    // coalesce with next
    char *next_addr = (char*)hdr + hdr->size;
    if ( next_addr + HEADER_SIZE <= heap_end ) {
        block_header *next = (block_header *)next_addr;
        if (!next->allocated) {
            if (next->prev) next->prev->next = next->next;
            if (next->next) next->next->prev = next->prev;
            if (free_list == next) free_list = next->next;
            
            hdr->size += next->size;
            write_footer(hdr);
        }
    }

    // coalesce with prev
    if ((char*)hdr - FOOTER_SIZE >= heap_base) {
        block_footer *prev_ftr = (block_footer *)((char *)hdr - FOOTER_SIZE);
        if (!prev_ftr->allocated) {
            block_header *prev = (block_header *)((char *)hdr - prev_ftr->size);
            
            if (prev->prev) prev->prev->next = prev->next;
            if (prev->next) prev->next->prev = prev->prev;
            if (free_list == prev) free_list = prev->next;
            
            prev->size += hdr->size;
            write_footer(prev);
            hdr = prev;
        }
    }

    // re‑insert hdr into the free‑list in address order
    hdr->prev = hdr->next = NULL;
    if (!free_list) {
        free_list = hdr;
    } else {
        block_header *cur = free_list, *p = NULL;
        while (cur && cur < hdr) {
            p   = cur;
            cur = cur->next;
        }
        hdr->prev = p;
        hdr->next = cur;
        if (p)       p->next = hdr;
        else         free_list = hdr;
        if (cur)     cur->prev = hdr;
    }
}
