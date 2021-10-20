#ifndef STORAGE_H
#define STORAGE_H

#include <pthread.h>

#include "utils/include/icl_hash.h"
#include "utils/include/list.h"
#include "utils/include/utilities.h"

// #define MAX_PATH 1024
#define MAX_OWNERS 10

#define STORAGE_OK          (0)
#define E_INVALID_PARAM     (-2)
#define E_MAX_FILES         (-3)
#define E_NOEXIST           (-4)
#define E_NOPERMISSION      (-5)
#define E_EXIST_ALREADY     (-6) 
#define E_NOINITIALIZE      (-7)

typedef struct _file_t {

    char    path[MAX_PATH];

    int     owners[MAX_OWNERS];       
    int     locked;

    void*   contents;
    size_t  size;

} file_t;

typedef struct _storage_t {

    int is_free;
    int max_capacity;
    int curr_capacity;

    int max_no_files;
    int curr_no_files;
    

    icl_hash_t *files;
    list_t *files_list;

    pthread_mutex_t storage_lock;
    pthread_cond_t  storage_available;

} storage_t;

/* Main functions */

storage_t*
storage_create(int max_capacity, int max_no_files);

int
storage_destroy(storage_t *storage);

int
storage_open_file(int client_id, storage_t *storage, char *pathname, int flags);

int
storage_close_file(int client_id, storage_t *storage, char *pathname);

int
storage_write_file(int client_id, storage_t *storage, char *pathname, size_t data_size, void* data);

int
storage_read_file(int client_id, storage_t *storage, char *pathname, size_t *buf_size, void** read_buf);

int
storage_remove_file(int client_id, storage_t *storage, char *pathname);

int
storage_lock_file(int client_id, storage_t *storage, char *pathname);

int
storage_unlock_file(int client_id, storage_t *storage, char *pathname);

int
storage_dump(storage_t *storage, FILE* stream);


/* Utilities */

void
print_file(file_t *f, FILE *stream);

void
free_key(void *key);

void
free_data(void *data);

bool
is_owner(int client_fd, int *owners);

void
add_owner(int client_fd, int *owners);

void
remove_owner(int client_fd, int *owners);

bool
is_locked_by(int client_fd, file_t *f);


#endif