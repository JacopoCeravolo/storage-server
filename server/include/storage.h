#ifndef STORAGE_H
#define STORAGE_H

#include "server/include/icl_hash.h"

typedef struct _file_t {
    size_t  size;
    void*   contents;
} file_t;

typedef struct _storage_t {

    int max_capacity;
    int curr_capacity;

    int max_no_files;
    int curr_no_files;

    icl_hash_t *files;

} storage_t;

storage_t*
create_storage(int max_capacity, int max_no_files);

int
write_file(storage_t *storage, char *pathname, size_t size, void* contents);

void*
read_file(storage_t *storage, char *pathname);

#endif