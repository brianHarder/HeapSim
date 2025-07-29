#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef struct block_header {
    int size;
    int allocated;
    struct block_header *next;
    struct block_header *prev;
} block_header;

typedef struct block_footer {
    int size;
    int allocated;
} block_footer;

#define HEADER_SIZE   sizeof(block_header)
#define FOOTER_SIZE   sizeof(block_footer)
#define OVERHEAD      (HEADER_SIZE + FOOTER_SIZE)

extern block_header *free_list;
extern block_header *heap_start;
extern int heap_size;

int my_init(int size_of_region);
void *first_fit(int size_of_payload);
void *best_fit(int size_of_payload);
void *worst_fit(int size_of_payload);
void *next_fit(int size_of_payload);
void *buddy_system(int size_of_payload);
void buddy_free(void *ptr);
void free_block(void *ptr);

extern block_header *free_bins[];
extern int max_order;

#endif