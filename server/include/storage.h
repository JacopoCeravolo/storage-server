#ifndef STORAGE_H
#define STORAGE_H

#include <pthread.h>

#include "server/include/icl_hash.h"
#include "utils/include/utilities.h"

#define STORAGE_OK      0
#define E_MAX_FILES     -3
#define E_NOEXIST       -4
#define E_NOPERMISSION  -5
#define E_EXIST_ALREADY -6 
#define E_NOINITIALIZE  -7

typedef struct _file_t {

    char      path[MAX_PATH];
    int       *owners;
    int       locked;
    int       opened_by;
    void*     contents;
    size_t    size;

} file_t;

typedef struct _storage_t {

    int is_free;
    int max_capacity;
    int curr_capacity;

    int max_no_files;
    int curr_no_files;

    icl_hash_t *files;

    pthread_mutex_t storage_lock;
    pthread_cond_t  storage_available;

} storage_t;

storage_t*
create_storage(int max_capacity, int max_no_files);

int
open_file(storage_t *storage, char *pathname, int flags, int client_id);

int
write_file(storage_t *storage, char *pathname, size_t size, void* contents, int client_id);

void*
read_file(storage_t *storage, char *pathname);

int
storage_dump(storage_t *storage, FILE* stream);

#endif