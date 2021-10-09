#ifndef WORKER_H
#define WORKER_H

#include "utils/include/protocol.h"

typedef struct _worker_arg_t {

    int         worker_id;
    long        client_fd;
    long        termina;

} worker_arg_t;

typedef struct _worker_res_t {

} worker_res_t;

void* 
worker_thread(void* args);

#endif