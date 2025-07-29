#include "allocator.h"
#include "cli.h"
#include "visualize.h"
#include <stdio.h>
#include <string.h>

#define VIS_WIDTH    60
#define MAX_BLOCKS  128

void print_heap(void) {
    block_header *blocks[MAX_BLOCKS];
    int sizes[MAX_BLOCKS];
    const char *names[MAX_BLOCKS];
    int n = 0;

    char *base  = (char*)heap_start;
    char *limit = base + heap_size;
    block_header *cur = heap_start;

    while ((char*)cur < limit && n < MAX_BLOCKS) {
        if (cur->size <= 0 || (char*)cur + cur->size > limit) {
            break;
        }

        blocks[n] = cur;
        sizes[n]  = cur->size;
        if (cur->allocated) {
            void *payload = (char*)cur + HEADER_SIZE;
            names[n] = find_name_by_ptr(payload);
        } else {
            names[n] = NULL;
        }
        n++;
        cur = (block_header*)((char*)cur + cur->size);
    }

    if (n == 0) {
        printf("[empty heap]\n");
        return;
    }

    int total = heap_size;
    int widths[MAX_BLOCKS];
    int sum = 0;
    for (int i = 0; i < n; i++) {
        widths[i] = (sizes[i] * VIS_WIDTH) / total;
        if (widths[i] < 1) widths[i] = 1;
        sum += widths[i];
    }
    
    for (int i = 0; i < n; i++) {
        if (names[i]) {
            char temp_label[64];
            snprintf(temp_label, sizeof(temp_label), "%s(%d)", names[i], sizes[i]);
            int min_width = strlen(temp_label);
            if (widths[i] < min_width) {
                int needed = min_width - widths[i];
                for (int j = 0; j < n && needed > 0; j++) {
                    if (j != i && widths[j] > 1) {
                        int can_take = widths[j] - 1;
                        if (can_take > needed) can_take = needed;
                        widths[j] -= can_take;
                        widths[i] += can_take;
                        needed -= can_take;
                    }
                }
            }
        }
    }
    
    sum = 0;
    for (int i = 0; i < n; i++) {
        sum += widths[i];
    }
    
    int diff = VIS_WIDTH - sum;
    for (int i = 0; diff != 0 && i < n; i++) {
        if (diff > 0) {
            widths[i]++; diff--;
        } else if (widths[i] > 1) {
            widths[i]--; diff++;
        }
    }

    // top border
    for (int i = 0; i < n; i++) {
        putchar('+');
        for (int j = 0; j < widths[i]; j++) putchar('-');
    }
    puts("+");

    for (int i = 0; i < n; i++) {
        putchar('|');
        char label[64];
        if (names[i]) {
            snprintf(label, sizeof(label), "%s(%d)", names[i], sizes[i]);
        } else {
            snprintf(label, sizeof(label), "%d", sizes[i]);
        }
        int L = strlen(label);
        
        if (L > widths[i]) {
            memmove(label, label + L - widths[i], widths[i]);
            label[widths[i]] = '\0';
            L = widths[i];
        }
        
        int pad = widths[i] - L;
        int left = pad/2, right = pad - left;
        for (int j = 0; j < left; j++) putchar(' ');
        fputs(label, stdout);
        for (int j = 0; j < right; j++) putchar(' ');
    }
    puts("|");

    for (int i = 0; i < n; i++) {
        putchar('+');
        for (int j = 0; j < widths[i]; j++) putchar('-');
    }
    puts("+\n");
}
