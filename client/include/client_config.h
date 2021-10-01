#ifndef CLIENT_CONFIG_H
#define CLIENT_CONFIG_H

#include <stddef.h>
#include <stdbool.h>

#include "utils/include/utilities.h"

#define CLIENT_OPTIONS      ":f:t:w:W:D:r:R:d:l:u:c:ph"
#define DIRECTORY_NOT_SET   "###"

#define MAX_ARGS            10   


/* A client configuration */
typedef struct _config_t {
    int     client_id;

    char    socket_name[MAX_PATH];
    char    reading_dir[MAX_PATH];
    char    expelled_dir[MAX_PATH];

    bool    verbose;
    bool    print_helper;
    long    wait_time;
} config_t;


extern config_t *config;

#endif