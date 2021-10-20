#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

bool
default_cmp(void* e1, void* e2)
{
    return (e1 == e2) ? true : false;
}

void
default_free(void* ptr)
{
    if (ptr) free(ptr);
}

void
default_print(void* e, FILE* stream)
{
    fprintf(stream, "Entry: %p\n", e);
}

list_t*
list_create(bool (*cmp)(void*, void*), void (*free_fun)(void*), void (*print)(void*, FILE*))
{
    list_t *list = calloc(1, sizeof(list_t));
    if (list == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    list->length = 0;
    list->head = list->tail = NULL;

    list->cmp = (*cmp == NULL) ? default_cmp : cmp;
    list->free_fun = (*free_fun == NULL) ? default_free : free_fun;
    list->print = (*print == NULL) ? default_print : print;
    return list;
}

int
list_destroy(list_t *list)
{
    //if (list->length > 0) {
        node_t *tmp;
        while (list->head != NULL) {
            tmp = list->head;
            list->head = list->head->next;
            list->free_fun(tmp->data);
            free(tmp);
        }
    //}
    free(list);
    return 0;
}

int
list_insert_tail(list_t *list, void* to_insert)
{
    if (list->head == NULL) {
        list->head = calloc(1, sizeof(node_t));
        if (list->head == NULL) {
            errno = ENOMEM;
            return -1;
        }
        list->head->data = to_insert;
        list->head->next = NULL;
        list->length++;
        return 0;
    } else {
        node_t *curr = list->head;
        while(curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = calloc(1, sizeof(node_t));
        if (curr->next == NULL) {
            errno = ENOMEM;
            return -1;
        }
        curr->next->data = to_insert;
        curr->next->next = NULL;
        list->length++;
        return 0;
    }
}

int 
list_insert_head(list_t *list, void* to_insert)
{
    node_t *new = calloc(1, sizeof(node_t));
    if (new == NULL) {
        errno = ENOMEM;
        return -1;
    }
    new->data = to_insert;
    new->next = list->head;
    list->head = new;
    list->length++;
    return 0;
}

int
list_insert_at_index(list_t *list, void* to_insert, int index)
{
    node_t *prev, *curr = list->head;
    while (index > 0 && curr != NULL) {
        prev = curr;
        curr = curr->next;
        index--;
    }

    node_t *new = calloc(1, sizeof(node_t));
    if (new == NULL) {
        errno = ENOMEM;
        return -1;
    }

    new->data = to_insert;
    new->next = curr;
    prev->next = new;
    list->length++;
    return 0;
}

int 
list_remove_head(list_t *list, void** to_return)
{
    if(list->head == NULL) {
        return 0;
    }

    node_t* tmp = list->head;
    list->head = list->head->next;
    *to_return = (tmp->data);
    free(tmp);
    list->length--;
    return 0;
}

bool
list_is_empty(list_t *list)
{
    if (list == NULL || list->head == NULL ||
        list->length == 0) {
        return true;
    } else {
        return false;
    }
}

int
list_index_of(list_t *list, void* elem)
{
    int index = 0;
    node_t *curr = list->head;
    while (curr != NULL) {
        if (list->cmp(curr->data, elem)) return index;
        curr = curr->next;
        index++;
    }
    return -1;
}

void
list_map(list_t *list, void (*fun)(void*))
{
    node_t *curr = list->head;
    while (curr != NULL) {
        fun(curr->data);
        curr = curr->next;
    }
}

void
list_dump(list_t *list, FILE *stream)
{
    node_t *curr = list->head;
    while (curr != NULL) {
        (list->print)(curr->data, stream);
        curr = curr->next;
    }
}
