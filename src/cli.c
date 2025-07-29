#include "allocator.h"
#include "cli.h"
#include "visualize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static var_t *var_list = NULL;
static void *(*alloc_func)(int) = NULL;
static void (*free_func)(void *) = NULL;

// select allocation algorithm
static int select_algorithm(void) {
    while (1) {
        printf("Select allocation algorithm:\n");
        printf("  1. first fit\n");
        printf("  2. best fit\n");
        printf("  3. worst fit\n");
        printf("  4. next fit\n");
        printf("  5. buddy system\n");
        printf("Enter choice (1-5): ");
        
        char choice[20];
        if (!fgets(choice, sizeof(choice), stdin)) {
            fprintf(stderr, "Failed to read algorithm choice\n");
            return 0;
        }
        
        choice[strcspn(choice, "\n")] = 0;
        
        if (strcmp(choice, "1") == 0 || strcasecmp(choice, "first fit") == 0) {
            alloc_func = first_fit;
            free_func = free_block;
            printf("Using first fit algorithm\n");
            return 1;
        } else if (strcmp(choice, "2") == 0 || strcasecmp(choice, "best fit") == 0) {
            alloc_func = best_fit;
            free_func = free_block;
            printf("Using best fit algorithm\n");
            return 1;
        } else if (strcmp(choice, "3") == 0 || strcasecmp(choice, "worst fit") == 0) {
            alloc_func = worst_fit;
            free_func = free_block;
            printf("Using worst fit algorithm\n");
            return 1;
        } else if (strcmp(choice, "4") == 0 || strcasecmp(choice, "next fit") == 0) {
            alloc_func = next_fit;
            free_func = free_block;
            printf("Using next fit algorithm\n");
            return 1;
        } else if (strcmp(choice, "5") == 0 || strcasecmp(choice, "buddy system") == 0 || strcasecmp(choice, "buddy") == 0) {
            alloc_func = buddy_system;
            free_func = buddy_free;
            printf("Using buddy system algorithm\n");
            return 1;
        } else {
            printf("Invalid choice. Please enter 1, 2, 3, 4, 5, or the algorithm name.\n");
        }
    }
}

void add_var(const char *name, void *ptr) {
    var_t *v = malloc(sizeof(var_t));
    v->name = strdup(name);
    v->ptr  = ptr;
    v->next = var_list;
    var_list = v;
}

void remove_var(const char *name) {
    var_t *prev = NULL, *cur = var_list;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else      var_list   = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur  = cur->next;
    }
}

void *find_var(const char *name) {
    var_t *cur = var_list;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur->ptr;
        cur = cur->next;
    }
    return NULL;
}

const char *find_name_by_ptr(void *ptr) {
    var_t *cur = var_list;
    while (cur) {
        if (cur->ptr == ptr) return cur->name;
        cur = cur->next;
    }
    return NULL;
}

void free_all_vars(void) {
    var_t *cur = var_list;
    while (cur) {
        var_t *next = cur->next;
        free(cur->name);
        free(cur);
        cur = next;
    }
    var_list = NULL;
}

int main(int argc, char **argv) {
    int sz = 8192;  // default heap size
    if (argc == 2) {
        sz = atoi(argv[1]);
        if (sz <= 0) {
            fprintf(stderr, "Invalid heap size '%s'\n", argv[1]);
            return 1;
        }
    } else if (argc > 2) {
        fprintf(stderr, "Usage: %s [heap_size]\n", argv[0]);
        return 1;
    }

    if (!select_algorithm()) {
        return 1;
    }

    if (my_init(sz) != 0) {
        fprintf(stderr, "Failed to initialize heap of %d bytes\n", sz);
        return 1;
    }
    printf("Heap initialized to %d bytes\n", heap_size);

    char line[256];
    while (1) {
        printf(">>> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;
        if (!*line) continue;

        if (strcmp(line, "exit") == 0) {
            printf("Closing HeapSim ...\n");
            break;
        }
        if (strcmp(line, "help") == 0) {
            printf(
                "Commands:\n"
                "  allocate(<var>,<sz>)  allocate bytes\n"
                "  free(<var>)           free variable\n"
                "  vis                   visualize heap\n"
                "  switch                switch allocation algorithm\n"
                "  exit                  quit\n"
            );
            continue;
        }

        if (strcmp(line, "switch") == 0) {
            free_all_vars();
            
            int current_heap_size = heap_size;
            if (my_init(current_heap_size) != 0) {
                fprintf(stderr, "Failed to reinitialize heap of %d bytes\n", current_heap_size);
                return 1;
            }
            
            if (!select_algorithm()) {
                return 1;
            }
            continue;
        }

        if (strncmp(line, "allocate(", 9) == 0) {
            char var[64];
            int req;
            if (sscanf(line, "allocate(%63[^,],%d)", var, &req) == 2 && req > 0) {
                char *p = var; while (*p == ' ') p++;
                char *e = var + strlen(var) - 1; while (e > var && *e == ' ') *e-- = 0;

                void *ptr = alloc_func(req);
                if (ptr) {
                    add_var(p, ptr);
                    printf("%s = %p\n", p, ptr);
                } else {
                    printf("Allocation failed\n");
                }
            } else {
                printf("Invalid allocate syntax\n");
            }
            continue;
        }

        if (strncmp(line, "free(", 5) == 0) {
            char var[64];
            if (sscanf(line, "free(%63[^)])", var) == 1) {
                char *p = var; while (*p == ' ') p++;
                char *e = var + strlen(var) - 1; while (e > var && *e == ' ') *e-- = 0;

                void *ptr = find_var(p);
                if (ptr) {
                    free_func(ptr);
                    remove_var(p);
                    printf("Freed %s\n", p);
                } else {
                    printf("Variable not found\n");
                }
            } else {
                printf("Invalid free syntax\n");
            }
            continue;
        }

        if (strcmp(line, "vis") == 0) {
            print_heap();
            continue;
        }

        printf("Unknown command. Type help for commands.\n");
    }

    free_all_vars();
    return 0;
}
