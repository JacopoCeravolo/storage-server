#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "server/include/storage.h"

#include "utils/include/logger.h"
#include "utils/include/utilities.h"

void
print_file(file_t *f)
{
    printf("FILE: %s\n" 
           "C_ID: %d\n"
           "SIZE: %ld\n", f->path, f->opened_by, f->size);
}

storage_t*
create_storage(int max_capacity, int max_no_files)
{
    storage_t *storage = malloc(sizeof(storage_t));
    if (storage == NULL) {
        return NULL;
    }
    storage->is_free = 1;
    storage->max_capacity = max_capacity;
    storage->curr_capacity = 0;

    storage->max_no_files  = max_no_files;
    storage->curr_no_files = 0;

    storage->files = icl_hash_create(max_no_files, hash_pjw, string_compare);
    if (storage->files == NULL) {
        return NULL;
    }

    if ((pthread_mutex_init(&(storage->storage_lock), NULL) != 0) ||
	            (pthread_cond_init(&(storage->storage_available), NULL) != 0)) {
	    return NULL;
    }
     
    return storage;
}

// TODO: destroy_storage

int
open_file(storage_t *storage, char *pathname, int flags, int client_id)
{
    if (storage == NULL) {
        LOG_ERROR("storage should be initialized\n");
        return E_NOINITIALIZE;
    }

    if (flags == O_CREATE) {

        file_t *new_file = malloc(sizeof(file_t));

        strcpy(new_file->path, pathname);
        new_file->size = 0;
        new_file->locked = 1;
        new_file->opened_by = client_id;
        
        icl_entry_t *file_entry = icl_hash_insert(storage->files, pathname, new_file);
        if (file_entry == NULL) {
            // LOG_ERROR("could not add entry to file table\n");
            // errno = EEXIST;
            return E_EXIST_ALREADY;
        }
        storage->curr_no_files++;
        
        return STORAGE_OK;
        // new_file->owners = malloc(sizeof(int) * MAX_OWNERS);
    }
    return STORAGE_OK;
}

int
write_file(storage_t *storage, char *pathname, size_t size, void* contents, int client_id)
{
    if (storage == NULL) {
        LOG_ERROR("storage should be initialized\n");
        return E_NOINITIALIZE;
    }

    if (storage->files == NULL) {
        LOG_ERROR("storage file table should be initialized\n");
        return E_NOINITIALIZE;
    }

    file_t *file = malloc(sizeof(file_t));
    if (file == NULL) return -1;

    file = (file_t*)icl_hash_find(storage->files, pathname);
    if (file == NULL) {
        // errno = ENOENT;
        return E_NOEXIST;
    }
    if (client_id == file->opened_by) {
        LOG_INFO(CLIENT "has permission to write file [%s]\n", client_id, pathname);

        file->size = size;
        file->contents = contents;

        // print_file(file);

        if (icl_hash_update_insert(storage->files, pathname, file, NULL) == NULL) {
            return -1;
        }
        
        storage->curr_capacity -= size;
        } else {
            // errno = EACCES;
            return E_NOPERMISSION;
        }
    return STORAGE_OK;
}

void*
read_file(storage_t *storage, char *pathname) 
{
    file_t *file = malloc(sizeof(file_t));
    file->contents = malloc(MAX_BUFFER);
    if (file == NULL) return NULL;

    file = (file_t*)icl_hash_find(storage->files, pathname);
    // LOG_DEBUG("found successfull\n");
    // LOG_INFO("%s\n", (char*)file->contents);
    return file->contents;
}

int
storage_dump(storage_t *storage, FILE* stream)
{
    icl_entry_t *bucket, *curr;
    int i;

    if(!storage->files) return -1;

    for(i=0; i<storage->files->nbuckets; i++) {
        bucket = storage->files->buckets[i];
        for(curr=bucket; curr!=NULL; ) {
            if(curr->key) {
                fprintf(stream, "\n***********************\n");
                file_t *f = (file_t*)curr->data;
                fprintf(stream, "  ENTRY: %s\n  SIZE: %ld (bytes)\n", (char *)curr->key, f->size);
                fprintf(stream, "***********************\n");
            
            }
            curr=curr->next;
        }
    }

    return 0;
}
