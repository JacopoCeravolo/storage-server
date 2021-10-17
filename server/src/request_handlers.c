#include "server/include/storage.h"
#include "server/include/request_handlers.h"
#include "utils/include/logger.h"

int
open_file_handler(int client_id, storage_t *storage, char *pathname, int flags, void* reply_buffer, size_t *buf_size)
{
    LOCK_RETURN(&(storage->storage_lock), -1);
    
    while (!(storage->is_free)) {
        pthread_cond_wait(&(storage->storage_available), &(storage->storage_lock));
    }
    storage->is_free = 0;

    int result;

    result = storage_open_file(client_id, storage, pathname, flags);

    switch (result) {
        case E_EXIST_ALREADY: {
            LOG_ERROR(CLIENT "file [%s] exists already and should not be opened with O_CREATE\n", client_id, pathname);
            *buf_size = strlen("File already in storage") + 1;
            memcpy(reply_buffer, "File already in storage", *buf_size);
            //strcpy(reply_buffer, "File already in storage");
            break;
        }
        case E_NOEXIST: {
            LOG_ERROR(CLIENT "file [%s] doesn't exists and should be opened with O_CREATE\n", client_id, pathname);
            *buf_size = strlen("File doesn't exist") + 1;
            memcpy(reply_buffer, "File doesn't exist", *buf_size);
            // strcpy(reply_buffer, "File doesn't exist");
            break;
        }
        case E_NOINITIALIZE: {
            LOG_FATAL("storage is not initialized, exiting..\n");
            *buf_size = strlen("FATAL: storage is not initialized") + 1;
            memcpy(reply_buffer, "FATAL: storage is not initialized", *buf_size);
            // strcpy(reply_buffer, "FATAL: storage is not initialized");
            exit(EXIT_FAILURE); // TERRIBLE WAY OF EXITING
        }
        case STORAGE_OK: {
            LOG_INFO(CLIENT "file [%s] successfully opened\n", client_id, pathname);
            *buf_size = strlen("Open file was successfull") + 1;
            memcpy(reply_buffer, "Open file was successfull", *buf_size);
            // strcpy(reply_buffer, "Open file was successfull");
            break;
        }
        
        default: {
            LOG_ERROR(CLIENT "INTERNAL ERROR: %s\n", client_id, strerror(errno));
            break;
        }
    }

    storage->is_free = 1;
    pthread_cond_signal(&(storage->storage_available));
    UNLOCK_RETURN(&(storage->storage_lock), -1);
    return result;
}

int 
write_file_handler(int client_id, storage_t *storage, char *pathname, size_t size, void* contents, void* reply_buffer, size_t *buf_size)
{
    LOCK_RETURN(&(storage->storage_lock), -1);
    
    while (!(storage->is_free)) {
        pthread_cond_wait(&(storage->storage_available), &(storage->storage_lock));
    }
    storage->is_free = 0;

    int result;

    result = storage_write_file(client_id, storage, pathname, size, contents);

    switch (result) {
        case E_NOPERMISSION: {
            LOG_ERROR(CLIENT "doesn't have parmission to write file [%s]\n", client_id, pathname);
            *buf_size = strlen("Permission denied") + 1;
            memcpy(reply_buffer, "Permission denied", *buf_size);
            // strcpy((char*)reply_buffer, "Permission denied");
            break;
        }
        case E_NOEXIST: {
            LOG_ERROR(CLIENT "file [%s] doesn't exists\n", client_id, pathname);
            *buf_size = strlen("File doesn't exist") + 1;
            memcpy(reply_buffer, "File doesn't exist", *buf_size);
            // strcpy((char*)reply_buffer, "File doesn't exist");
            break;
        }
        case E_NOINITIALIZE: {
            LOG_FATAL("storage is not initialized, exiting..\n");
            *buf_size = strlen("FATAL: storage is not initialized") + 1;
            memcpy(reply_buffer, "FATAL: storage is not initialized", *buf_size);
            // strcpy((char*)reply_buffer, "FATAL: storage is not initialized");
            exit(EXIT_FAILURE); // TERRIBLE WAY OF EXITING
        }
        case STORAGE_OK: {
            LOG_INFO(CLIENT "file [%s] successfully written\n", client_id, pathname);
            *buf_size = strlen("Write file was successfull") + 1;
            memcpy(reply_buffer, "Write file was successfull", *buf_size);
            // strcpy((char*)reply_buffer, "Write file was successfull");
            break;
        }

        default: {
            LOG_ERROR(CLIENT "INTERNAL ERROR: %s\n", client_id, strerror(errno));
            break;
        }
    }
    storage->is_free = 1;
    pthread_cond_signal(&(storage->storage_available));
    UNLOCK_RETURN(&(storage->storage_lock), -1);
    return result;
}

int
read_file_handler(int client_id, storage_t *storage, char *pathname, void* reply_buffer, size_t *buf_size)
{
    LOCK_RETURN(&(storage->storage_lock), -1);
    
    while (!(storage->is_free)) {
        pthread_cond_wait(&(storage->storage_available), &(storage->storage_lock));
    }
    storage->is_free = 0;

    int result;

    result = storage_read_file(client_id, storage, pathname, buf_size, reply_buffer);

    switch (result) {
        case E_NOEXIST: {
            LOG_ERROR(CLIENT "file [%s] doesn't exists\n", client_id, pathname);
            *buf_size = strlen("File doesn't exist") + 1;
            memcpy(reply_buffer, "File doesn't exist", *buf_size);
            // strcpy((char*)reply_buffer, "File doesn't exist");
            break;
        }
        case E_NOINITIALIZE: {
            LOG_FATAL("storage is not initialized, exiting..\n");
            *buf_size = strlen("FATAL: storage is not initialized") + 1;
            memcpy(reply_buffer, "FATAL: storage is not initialized", *buf_size);
            // strcpy((char*)reply_buffer, "FATAL: storage is not initialized");
            exit(EXIT_FAILURE); // TERRIBLE WAY OF EXITING
        }
        case STORAGE_OK: {
            LOG_INFO(CLIENT "file [%s] successfully read\n", client_id, pathname);
            // memcpy(reply_buffer, "Read file was successfull", strlen("Read file was successfull") + 1);
            // strcpy((char*)reply_buffer, "Write file was successfull");
            break;
        }

        default: {
            LOG_ERROR(CLIENT "INTERNAL ERROR: %s\n", client_id, strerror(errno));
            break;
        }
    }
    storage->is_free = 1;
    pthread_cond_signal(&(storage->storage_available));
    UNLOCK_RETURN(&(storage->storage_lock), -1);
    return result;
}

list_t*
read_n_file_handler(storage_t *storage, int N, void* reply_buffer, size_t *buf_size)
{
    LOCK_RETURN(&(storage->storage_lock), NULL);

    while (!(storage->is_free)) {
        pthread_cond_wait(&(storage->storage_available), &(storage->storage_lock));
    }
    storage->is_free = 0;

    list_t *copy = storage->files_list;
    list_t *file_list = list_create(NULL, NULL, NULL);
    
    for (int i = 0; i < N; i++) {
        void* tmp;
        list_remove_head(storage->files_list, &tmp);
        list_insert_head(file_list, tmp);
    }

    storage->files_list = copy;
    storage->is_free = 1;
    pthread_cond_signal(&(storage->storage_available));
    UNLOCK_RETURN(&(storage->storage_lock), NULL);
    return file_list;

}