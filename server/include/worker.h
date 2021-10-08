#ifndef WORKER_H
#define WORKER_H

#include <pthread.h>

#include "utils/include/queue.h"

typedef struct _lqueue_t {
    queue_t *queue;
    int     queue_size;
    pthread_mutex_t lock;
    pthread_cond_t  notify;
} lqueue_t;

typedef struct _worker_arg_t {

    int         worker_id;
    int         pipe_fd;
    lqueue_t    *requests;
    int         exit;

} worker_arg_t;

void*
worker_thread(void* args);

#endif