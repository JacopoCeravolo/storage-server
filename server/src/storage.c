#include <stdio.h>
#include <stdlib.h>

#include "server/include/storage.h"

#include "utils/include/logger.h"
#include "utils/include/utilities.h"

storage_t*
create_storage(int max_capacity, int max_no_files)
{
    storage_t *storage = malloc(sizeof(storage_t));
    if (storage == NULL) {
        return NULL;
    }
    storage->max_capacity = max_capacity;
    storage->curr_capacity = 0;

    storage->max_no_files  = max_no_files;
    storage->curr_no_files = 0;

    storage->files = icl_hash_create(max_no_files, hash_pjw, string_compare);
    if (storage->files == NULL) {
        return NULL;
    }
     
    return storage;
}

// TODO: destroy_storage

int
write_file(storage_t *storage, char *pathname, size_t size, void* contents)
{
    if (storage == NULL) {
        LOG_ERROR("storage should be initialized\n");
        return -1;
    }
    
    file_t *file = malloc(sizeof(file_t));
    if (file == NULL) return -1;

    file->size = size;
    file->contents = contents;

    if (storage->files == NULL) {
        LOG_ERROR("storage files table should be initialized\n");
        return -1;
    }
    icl_entry_t *file_entry;
    file_entry = icl_hash_insert(storage->files, pathname, file);
    if (file_entry == NULL) {
        LOG_ERROR("could not add entry to file table\n");
        return -1;
    }
    
    storage->curr_no_files++;
    storage->curr_capacity -= size;
    return 0;
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
                fprintf(stream, "* ENTRY: %s\n* SIZE: %ld (bytes)\n", (char *)curr->key, f->size);
                fprintf(stream, "***********************\n");
            
            }
            curr=curr->next;
        }
    }

    return 0;
}