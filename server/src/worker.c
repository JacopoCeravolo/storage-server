#include <assert.h>
#include <unistd.h>


#include "server/include/worker.h"
#include "utils/include/logger.h"
#include "utils/include/protocol.h"
#include "server/include/server_config.h"

int
open_file_handler(storage_t *storage, char *pathname, int flags, int client_id, char *reply_buffer)
{
    LOCK_RETURN(&(storage->storage_lock), -1);
    
    while (!(storage->is_free)) {
        pthread_cond_wait(&(storage->storage_available), &(storage->storage_lock));
    }
    storage->is_free = 0;

    int result;

    result = open_file(storage, pathname, O_CREATE, client_id);

    switch (result) {
        case E_EXIST_ALREADY: {
            LOG_ERROR(CLIENT "file [%s] exists already and should not be opened with O_CREATE\n", client_id, pathname);
            strcpy(reply_buffer, "File already in storage");
            break;
        }
        case E_NOEXIST: {
            LOG_ERROR(CLIENT "file [%s] doesn't exists and should be opened with O_CREATE\n", client_id, pathname);
            strcpy(reply_buffer, "File doesn't exist");
            break;
        }
        case E_NOINITIALIZE: {
            LOG_FATAL("storage is not initialized, exiting..\n");
            strcpy(reply_buffer, "FATAL: storage is not initialized");
            exit(EXIT_FAILURE); // TERRIBLE WAY OF EXITING
        }
        case STORAGE_OK: {
            LOG_INFO(CLIENT "file [%s] successfully opened\n", client_id, pathname);
            strcpy(reply_buffer, "Open file was successfull");
            break;
        }
    }
    storage->is_free = 1;
    pthread_cond_signal(&(storage->storage_available));
    UNLOCK_RETURN(&(storage->storage_lock), -1);
    return result;
}

int 
write_file_handler(storage_t *storage, char *pathname, size_t size, void* contents, int client_id, char *reply_buffer)
{
    LOCK_RETURN(&(storage->storage_lock), -1);
    
    while (!(storage->is_free)) {
        pthread_cond_wait(&(storage->storage_available), &(storage->storage_lock));
    }
    storage->is_free = 0;

    int result;

    result = write_file(storage, pathname, size, contents, client_id);

    switch (result) {
        case E_NOPERMISSION: {
            LOG_ERROR(CLIENT "doesn't have parmission to write file [%s]\n", client_id, pathname);
            strcpy(reply_buffer, "Permission denied");
            break;
        }
        case E_NOEXIST: {
            LOG_ERROR(CLIENT "file [%s] doesn't exists\n", client_id, pathname);
            strcpy(reply_buffer, "File doesn't exist");
            break;
        }
        case E_NOINITIALIZE: {
            LOG_FATAL("storage is not initialized, exiting..\n");
            strcpy(reply_buffer, "FATAL: storage is not initialized");
            exit(EXIT_FAILURE); // TERRIBLE WAY OF EXITING
        }
        case STORAGE_OK: {
            LOG_INFO(CLIENT "file [%s] successfully written\n", client_id, pathname);
            strcpy(reply_buffer, "Write file was successfull");
            break;
        }
    }
    storage->is_free = 1;
    pthread_cond_signal(&(storage->storage_available));
    UNLOCK_RETURN(&(storage->storage_lock), -1);
    return result;
}


void*
worker_thread(void* args)
{
    assert(args);

    // int worker_id = ((worker_arg_t*)args)->worker_id;
    int pipe_fd = ((worker_arg_t*)args)->pipe_fd;
    int exit = ((worker_arg_t*)args)->exit;
    lqueue_t *requests = ((worker_arg_t*)args)->requests;

    free(args);

    // LOG_INFO(WORKER "has started\n", worker_id);

    while (!exit) {

        int client_fd;
        LOCK_RETURN(&(requests->lock), NULL);
        while (requests->queue_size == 0) {
            pthread_cond_wait(&(requests->notify), &(requests->lock));
        }

        // CRITICAL SECTION
        dequeue(requests->queue, &client_fd);
        requests->queue_size--;
        UNLOCK_RETURN(&(requests->lock), NULL);
        // CRITICAL SECTION
        if (client_fd == -1) return NULL;

        message_t *message;

        if ((message = recv_message(client_fd)) == NULL) {
            LOG_ERROR("recv_message(): %s\n", strerror(errno));
            return NULL;
            // if (errno != EPIPE) return (void*)-1;
        }
    
        int         code;
        char        filename[MAX_PATH];
        size_t      size;
        code = message->header.code;
        size = message->header.msg_size;
        strcpy(filename, message->header.filename);

        char *reply_buffer = calloc(MAX_BUFFER, 1);
        int result;
        switch (code) {

            case REQ_OPEN: {
                result = open_file_handler(storage, message->header.filename, 
                                            O_CREATE, client_fd - 5, reply_buffer);
                break;
            }
            case REQ_WRITE: {
                result = write_file_handler(storage, message->header.filename, 
                                            size, message->body, client_fd - 5, reply_buffer);
                break;
            }
            default: {
                    strcpy(reply_buffer, "SUCCESS");
                    message = set_message(RES_SUCCESS, filename, strlen(reply_buffer) + 1, reply_buffer);
                    break;
            }
        }

        int res_code = ((result == 0) ? RES_SUCCESS : RES_ERROR);
        message = set_message(res_code, filename, strlen(reply_buffer) + 1, reply_buffer);

        if (send_message(client_fd, message) != 0) {
           LOG_ERROR("send_message(): %s\n", strerror(errno));
           return NULL;
        }

        LOG_INFO(CLIENT "%s on [%s] done\n", client_fd - 5, msg_code_to_str(code), filename);

        switch (code) {
            case REQ_END: {
                int end = -1;
                write(pipe_fd, &end, sizeof(int));
                LOG_INFO(CLIENT "has ended the connection\n",
                            client_fd - 5)
                break;
            }
            default: {
                write(pipe_fd, &client_fd, sizeof(int));
                break;
            }
        }
        free(message->body);
        free(message);
        free(reply_buffer);
    }
    return NULL;
}

