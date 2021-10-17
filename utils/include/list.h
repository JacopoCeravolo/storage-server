#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdbool.h>

typedef struct _node_t {
    void*   data;
    struct _node_t *next;
} node_t;

typedef struct _list_t {
    int     length;

    node_t  *head;
    node_t  *tail;

    bool    (*cmp)(void*, void*);
    void    (*free_fun)(void*);
    void    (*print)(void*, FILE*);

} list_t;

list_t*
list_create(bool cmp(void*, void*), void free_fun(void*), void print(void*, FILE*));

int
list_destroy(list_t *list);

int
list_insert_head(list_t *list, void* to_insert);

int 
list_insert_tail(list_t *list, void* to_insert);

int
list_insert_at_index(list_t *list, void* to_insert, int index);

int
list_remove_head(list_t *list, void** to_return);

bool
list_is_empty(list_t *list);

int
list_index_of(list_t *list, void* elem);

void
list_map(list_t *list, void fun(void*));

void
list_dump(list_t *list, FILE *stream);

#endif