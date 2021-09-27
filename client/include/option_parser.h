#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H

#include <getopt.h>
#include <stdlib.h>
#include <errno.h>

#include "client/include/client_config.h"

#include "utils/include/queue.h"

/* Enum type describing the different option available */
typedef enum _option_code {
    OPTION_READ     = 9,    // -r
    OPTION_WRITE    = 10,   // -W
    OPTION_RREAD    = 11,   // -R
    OPTION_RWRITE   = 12,   // -w
    OPTION_LOCK     = 13,   // -l
    OPTION_UNLOCK   = 14,   // -u
    OPTION_REMOVE   = 15    // -c
} option_code;

/* (String, Int) pair */
typedef struct _str_int_pair {
    char    dirname[1024];
    int     how_many;
} str_int_pair;

/* Type of an option */
typedef struct _option_t {
    option_code     code;                           // its code
    union {
        queue_t      *files_list;                    // list of arguments (for -r, -W, -l, -u, -c) TODO? change to 2D array
        int          how_many_files;                // number of files (for -R)
        str_int_pair w_arg;                         // directory and number of files (for -w)    
        // TODO: aggiungere -w nome, N (str, int)
    };
} option_t;

/* Data Structures */
extern queue_t  *option_list;
extern config_t *config;

const char*
option_to_str(option_code code);

int
parse_options(int argc,  const char *argv[], const char *options_str);

#endif