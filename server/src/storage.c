#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "server/include/storage.h"
#include "utils/include/utilities.h"


storage_t*
storage_create(int max_capacity, int max_no_files)
{
    storage_t *storage = calloc(1, sizeof(storage_t));
    if (storage == NULL) {
        return NULL;
    }
    storage->is_free = 1;
    storage->max_capacity = max_capacity;
    storage->curr_capacity = max_capacity;

    storage->max_no_files  = max_no_files;
    storage->curr_no_files = 0;

    storage->files_list = list_create(NULL, NULL, NULL);

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

int
storage_destroy(storage_t *storage)
{
    if (icl_hash_destroy(storage->files, NULL, free_data) != 0) {
        return -1;
    }

    pthread_mutex_destroy(&(storage->storage_lock));
    pthread_cond_destroy(&(storage->storage_available));
    list_destroy(storage->files_list);
    free(storage);
    return 0;
}

/**
 * Attempts to open the file according to passed flags, if successfull @param client_id 
 * is added to the owners list of the file.
 * 
 * @param client_fd client performing the request
 * @param storage the file storage
 * @param pathname the file name
 * @param flags opening modalities
 * 
 * @return  STORAGE_OK (0) on success
 *          E_NOINITIALIZE if storage is not initialized
 *          E_INVALID_PARAM if either client_fd < 0 or pathname = NULL or flags < 0
 *          E_EXIST_ALREADY if flags = O_CREATE and file already exists
 *          -1 if any internal error occurs (no memory,...)    
 */
int
storage_open_file(int client_id, storage_t *storage, char *pathname, int flags)
{
    /* Checks parameters */

    if (storage == NULL || client_id < 0 
        || pathname == NULL || flags < 0) {
        errno = EINVAL;
        return -1;
    }

    file_t *file = calloc(1, sizeof(file_t));
    if (file == NULL) {
        errno = ENOMEM;
        return -1;
    }

    /* Open file in create mode */

    if (CHK_FLAG(flags, O_CREATE)) {

        // fprintf(stderr, "Flag is create\n");
        /* Set file metadata */

        strcpy(file->path, pathname);

        memset(file->owners, 0, MAX_OWNERS * sizeof(int));
        add_owner(client_id, file->owners);

        file->size = 0;
        file->locked = 0;
        
        /* Add empty file to storage */

        if (icl_hash_insert(storage->files, pathname, file) == NULL) {
            free(file);
            errno = EEXIST;
            return -1;
        }

        //free(file_entry);
        storage->curr_no_files++;
        list_insert_head(storage->files_list, pathname);
    }

    if (CHK_FLAG(flags, O_READ)) {

        /* Check whether file exists */
        // fprintf(stderr, "Flag is read\n");

        file = (file_t*)icl_hash_find(storage->files, pathname);
        if (file == NULL) {
            errno = ENOENT;
            return -1;
        }

        if (file->locked != client_id) {
            errno = EPERM;
            return -1;
        }

        if (!is_owner(client_id, file->owners)) {
            add_owner(client_id, file->owners);
        }
    }

    if (CHK_FLAG(flags, O_LOCK)) {

        //fprintf(stderr, "Flag is lock\n");
        file = (file_t*)icl_hash_find(storage->files, pathname);
        if (file == NULL) {
            errno = ENOENT;
            return -1;
        }

        if (file->locked || !is_owner(client_id, file->owners)) {
            errno = EPERM;
            return -1;
        }

        return storage_lock_file(client_id, storage, pathname);
    }

    return 0;
}

/**
 * Attempts to close the file identified, if successfull @param client_id 
 * is removed from the owners list of the file.
 * 
 * @param client_fd client performing the request
 * @param storage the file storage
 * @param pathname the file name
 *
 * @return  STORAGE_OK (0) on success
 *          E_NOINITIALIZE if storage is not initialized
 *          E_INVALID_PARAM if either client_fd < 0 or pathname = NULL
 *          E_NO_EXISTS if file @param pathname doesn't exists
 *          E_NOPERMISSION if @param client_fd hasn't previously opened the file
 *          -1 if any internal error occurs (no memory,...)    
 */
int
storage_close_file(int client_id, storage_t *storage, char *pathname)
{
    /* Checks parameters */

    if (storage == NULL) {
        errno = EINVAL;
        return E_NOINITIALIZE;
    }

    if (client_id < 0 || pathname == NULL) {
        errno = EINVAL;
        return E_INVALID_PARAM;
    }

    /* Check whether file exists */
    
    file_t *file = calloc(1, sizeof(file_t));
    if (file == NULL) {
        errno = ENOMEM;
        return -1;
    }

    file = (file_t*)icl_hash_find(storage->files, pathname);
    if (file == NULL) {
        errno = ENOENT;
        return E_NOEXIST;
    }

    if (!is_owner(client_id, file->owners)) {
        //errno = ?
        return E_NOPERMISSION;
    }

    remove_owner(client_id, file->owners);

    if (icl_hash_update_insert(storage->files, pathname, file, NULL) == NULL) {
        return -1;
    }
    // free(file);
    return STORAGE_OK;
}

int
storage_write_file(int client_id, storage_t *storage, char *pathname, size_t data_size, void* data)
{
    /* Check parameters */

    if (storage == NULL || storage->files == NULL) {
        return E_NOINITIALIZE;
    }

    if (client_id < 0 || pathname == NULL 
        || data_size < 0 || data == NULL) {
        errno = EINVAL;
        return E_INVALID_PARAM;
    }

    /* Check whether file exists and is opened by client_id */

    file_t *file = calloc(1, sizeof(file_t));
    if (file == NULL) return -1;

    file = (file_t*)icl_hash_find(storage->files, pathname);
    if (file == NULL) {
        return E_NOEXIST;
    }

    if (!is_owner(client_id, file->owners)) {
        return E_NOPERMISSION;
    }

    /* if (file->locked != client_id) {
        return E_NOPERMISSION;
    } */
    
    file->size = data_size;
    file->contents = data;
    storage->curr_capacity -= data_size;

    if (icl_hash_update_insert(storage->files, pathname, file, NULL) == NULL) {
        // free(file->contents);
        // free(file);
        return -1;
    }

    return STORAGE_OK;
}

int
storage_read_file(int client_id, storage_t *storage, char *pathname, size_t *buf_size, void** read_buf)
{
    /* Checks parameters */

    if (storage == NULL) {
        errno = EINVAL;
        return E_NOINITIALIZE;
    }

    if (client_id < 0 || pathname == NULL) {
        errno = EINVAL;
        return E_INVALID_PARAM;
    }

    /* Check whether file exists and is opened by client_id */

    file_t *file = calloc(1, sizeof(file_t));
    if (file == NULL) return -1;

    file = (file_t*)icl_hash_find(storage->files, pathname);
    if (file == NULL) {
        return E_NOEXIST;
    }

    if (!is_owner(client_id, file->owners)) {
        return E_NOPERMISSION;
    }

    /* Check wheteher read_buf has enough memory to hold file contents */

    /* if (*buf_size < file->size) {
        void* tmp_ptr = realloc(read_buf, file->size);
        if (tmp_ptr == NULL) {
            errno = ENOMEM;
            free(file);
            return -1;
        }
        printf("reallocated\n");
        *read_buf = tmp_ptr;
    } */

    *buf_size = file->size;
    *read_buf = calloc(1, file->size);
    memcpy(*read_buf, file->contents, file->size);
    //memcpy(*read_buf, file->contents, file->size);
    return STORAGE_OK;
}

int
storage_lock_file(int client_id, storage_t *storage, char *pathname)
{
    /* Checks parameters */

    if (storage == NULL) {
        errno = EINVAL;
        return E_NOINITIALIZE;
    }

    if (client_id < 0 || pathname == NULL) {
        errno = EINVAL;
        return E_INVALID_PARAM;
    }

    /* Check whether file exists */
    
    /* file_t *file = calloc(1, sizeof(file_t));
    if (file == NULL) {
        errno = ENOMEM;
        return -1;
    } */

    file_t *file = (file_t*)icl_hash_find(storage->files, pathname);
    if (file == NULL) {
        errno = ENOENT;
        return E_NOEXIST;
    }

    if (!is_owner(client_id, file->owners) || file->locked) {
        //errno = ?
        return E_NOPERMISSION;
    }

    file->locked = client_id;

    if (icl_hash_update_insert(storage->files, pathname, file, NULL) == NULL) {
        return -1;
    }

    return STORAGE_OK;
}

int
storage_dump(storage_t *storage, FILE* stream)
{
    icl_entry_t *bucket, *curr;
    int i;

    if(!storage->files_list) return -1;

    fprintf(stream, "NO_FILES: %d\tCAPACITY: %d\n", storage->curr_no_files, storage->curr_capacity);
    fprintf(stream, "FILES: ");
    while (!list_is_empty(storage->files_list)) {
        char *tmp = calloc(1, MAX_PATH);
        list_remove_head(storage->files_list, (void**)tmp);
        fprintf(stream, "%s ", tmp);
        free(tmp);
    }
    fprintf(stream, "\n\n");
    if(!storage->files) return -1;

    for(i=0; i<storage->files->nbuckets; i++) {
        bucket = storage->files->buckets[i];
        for(curr=bucket; curr!=NULL; ) {
            if(curr->key) {
                fprintf(stream, "\n***********************\n");
                file_t *f = (file_t*)curr->data;
                print_file(f, stream);
                // fprintf(stream, "  ENTRY: %s\n  SIZE: %ld (bytes)\n", (char *)curr->key, f->size);
                fprintf(stream, "\n***********************\n");
            
            }
            curr=curr->next;
        }
    }

    return 0;
}



/* Utilities */

void
print_file(file_t *f, FILE *stream)
{
    fprintf(stream,
            "Name:   %s\n"
            "Size:   %ld\n"
            "Locked: %d\n"
            "Owners: ", f->path, f->size, f->locked);
    for (int i = 0; i < MAX_OWNERS; i++) {
        if (f->owners[i] != 0) fprintf(stream, "%d ", f->owners[i]);
    }
    fprintf(stream, 
            "\nCONTENTS: \n%s\n", (char*)f->contents);
}

void
free_key(void *key)
{
    free(key);
}

void
free_data(void *data)
{
    file_t *file = (file_t*)data;
    if (file->contents) free(file->contents);
    free(file);
}

bool
is_owner(int client_fd, int *owners)
{
    for (int i = 0; i < MAX_OWNERS; i++) {
        if (owners[i] == client_fd) return true;
    }
    return false;
}

void
add_owner(int client_fd, int *owners)
{
    for (int i = 0; i < MAX_OWNERS; i++) {
        if (owners[i] == 0) {
            owners[i] = client_fd;
            return;
        }
    }

    // should return int and check for correctness
}

void
remove_owner(int client_fd, int *owners) {
    
    for (int i = 0; i < MAX_OWNERS; i++) {
        if (owners[i] == client_fd) {
            owners[i] = 0;
            return;
        }
    }

    // should return int and check for correctness
}











/* int
storage_write_file(int client_id, storage_t *storage, char *pathname, size_t data_size, void* data)
{
    if (storage == NULL) {
        printf("storage should be initialized\n");
        return E_NOINITIALIZE;
    }

    if (storage->files == NULL) {
        printf("storage file table should be initialized\n");
        return E_NOINITIALIZE;
    }

    file_t *file = calloc(1, sizeof(file_t));
    if (file == NULL) return -1;

    file = (file_t*)icl_hash_find(storage->files, pathname);
    if (file == NULL) {
        // errno = ENOENT;
        // free(file);
        return E_NOEXIST;
    }
    if (client_id == file->opened_by) {
        printf("clien %d has permission to write file [%s]\n", client_id, pathname);

        file->size = data_size;
        //file->contents = calloc(1, size);
        //memcpy(file->contents, contents, size);
        file->contents = data;

        // print_file(file);

        if (icl_hash_update_insert(storage->files, pathname, file, NULL) == NULL) {
            // free(file->contents);
            // free(file);
            return -1;
        }
        
        storage->curr_capacity -= data_size;
    } else {
        // errno = EACCES;
        // free(file);
        return E_NOPERMISSION;
    }
    // free(file);
    return STORAGE_OK;
}

int
storage_read_file(int client_id, storage_t *storage, char *pathname, size_t *buf_size, void* read_buf)
{
    file_t *file = calloc(1, sizeof(file_t));
    // file->contents = calloc(1, MAX_BUFFER);
    if (file == NULL) {
        errno = ENOMEM;
        return -1;
    }

    file = (file_t*)icl_hash_find(storage->files, pathname);
    if (file == NULL) return E_NOEXIST;

    if (*size < file->size) {
        void* tmp_ptr = realloc(read_buf, file->size);
        if (tmp_ptr == NULL) {
            errno = ENOMEM;
            free(file);
            return -1;
        }
        read_buf = tmp_ptr;
    }

    *size = file->size;
    memcpy(read_buf, file->contents, file->size);
    // LOG_DEBUG("found successfull\n");
    // LOG_INFO("%s\n", (char*)file->contents);
    return STORAGE_OK;
}

int
storage_remove_file(int client_id, storage_t *storage, char *pathname)
{

}
*/
