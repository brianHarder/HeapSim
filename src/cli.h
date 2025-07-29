#ifndef CLI_H
#define CLI_H

typedef struct var_t {
    char *name;
    void *ptr;
    struct var_t *next;
} var_t;

void add_var(const char *name, void *ptr);
void remove_var(const char *name);
void *find_var(const char *name);
const char *find_name_by_ptr(void *ptr);
void free_all_vars(void);

#endif