#ifndef WORKER_H
#define WORKER_H

#include <pthread.h>

#include "utils/include/list.h"

typedef struct _llist_t {
    list_t *queue;
    int     queue_size;
    pthread_mutex_t lock;
    pthread_cond_t  notify;
} llist_t;

typedef struct _worker_arg_t {

    int         worker_id;
    int         pipe_fd;
    llist_t    *requests;
    int         exit;

} worker_arg_t;

void*
worker_thread(void* args);

#endif