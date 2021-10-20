#include <assert.h>
#include <unistd.h>


#include "server/include/worker.h"
#include "utils/include/logger.h"
#include "utils/include/protocol.h"
#include "server/include/server_config.h"
#include "server/include/request_handlers.h"

#define MSG_SUCCESS     "Operation successfull"
#define MSG_SUCCESS_LEN strlen(MSG_SUCCESS) + 1

int
open_file_handler1(int client_id, storage_t *storage, message_t *message, void** reply_buffer, size_t *buf_size)
{
    LOCK_RETURN(&(storage->storage_lock), -1);
    
    while (!(storage->is_free)) {
        pthread_cond_wait(&(storage->storage_available), &(storage->storage_lock));
    }
    storage->is_free = 0;

    int result;
    errno = 0;
    result = storage_open_file(client_id, storage, message->filename, *(int*)message->body);

    switch (result) {
        case 0: {
            *reply_buffer = malloc(MSG_SUCCESS_LEN);
            *buf_size = MSG_SUCCESS_LEN;
            memcpy(*reply_buffer, MSG_SUCCESS, MSG_SUCCESS_LEN);
            break;
        }
        case -1: {
            char err_buf[1024];
            strerror_r(errno, err_buf, 1024);
            *buf_size = strlen(err_buf) + 1;
            *reply_buffer = malloc(*buf_size);
            memcpy(*reply_buffer, err_buf, *buf_size);
            break;
        }
    }
    storage->is_free = 1;
    pthread_cond_signal(&(storage->storage_available));
    UNLOCK_RETURN(&(storage->storage_lock), -1); 

    // fprintf(stderr, "%d: message will be: %s\n",client_id, (char*)*reply_buffer);  
    return result;
}

void*
worker_thread(void* args)
{
    assert(args);

    // int worker_id = ((worker_arg_t*)args)->worker_id;
    int pipe_fd = ((worker_arg_t*)args)->pipe_fd;
    int exit = ((worker_arg_t*)args)->exit;
    llist_t *requests = ((worker_arg_t*)args)->requests;

    free(args);

    // LOG_INFO(WORKER "has started\n", worker_id);

    while (!exit) {

        int client_fd;
        LOCK_RETURN(&(requests->lock), NULL);
        while (requests->queue_size == 0) {
            pthread_cond_wait(&(requests->notify), &(requests->lock));
        }

        // CRITICAL SECTION
        // LOG_INFO("Worker dequeueing\n");
        void* tmp;
        list_remove_head(requests->queue, &tmp);
        client_fd = (int)tmp;
        // LOG_INFO("Worker dequeued %d\n", client_fd);
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
        print_message(message);
    
        int         code;
        char        filename[MAX_PATH];
        size_t      size;
        code = message->code;
        size = message->size;
        strcpy(filename, message->filename);

        void *reply_buffer = NULL;
        size_t buf_size = 0;

        int result;
        switch (code) {

            /* case REQ_OPEN: {
                result = open_file_handler1(client_fd, storage, message, &reply_buffer, &buf_size);
                break;
            }

            case REQ_WRITE: {
                result = write_file_handler(client_fd, storage, message->header.filename, 
                                            size, message->body, reply_buffer, &buf_size);
                break;
            }

            case REQ_READ: {
                result = read_file_handler(client_fd, storage, message->header.filename,
                                            reply_buffer, &buf_size);
                break;
            }

            case REQ_READ_N: {
                list_t *files_list;
                int N = *(int*)message->body;

                if (N == 0 || N > storage->curr_no_files) N = storage->curr_no_files;
                files_list = read_n_file_handler(storage, N, reply_buffer, &buf_size);
                if (files_list != NULL) {
                    char *to_send = malloc(N * MAX_PATH);
                    while (!list_is_empty(files_list)) {
                        char *tmp = malloc(MAX_PATH);
                        list_remove_head(files_list, (void**)tmp);
                        strcat(to_send, tmp);
                        strcat(to_send, ":");
                        free(tmp);
                    }
                    
                    result = 0;
                    buf_size = strlen(to_send) + 1;
                    memcpy(reply_buffer, to_send, buf_size);
                    list_destroy(files_list);
                    free(to_send);
                    break;
                } else {
                    result = -1;
                    buf_size = strlen("ERROR") + 1;
                    memcpy(reply_buffer, "ERROR", buf_size);
                    break;
                }
            }
             */
            default: {
                    reply_buffer = malloc(MSG_SUCCESS_LEN);
                    buf_size = MSG_SUCCESS_LEN;
                    memcpy(reply_buffer, MSG_SUCCESS, MSG_SUCCESS_LEN);
                    result = 0;
                    buf_size = strlen("SUCCESS") + 1;
                    memcpy(reply_buffer, "SUCCESS", buf_size);
                    break;
            }
        }

        int res_code = ((result == 0) ? RES_SUCCESS : RES_ERROR);
        result = send_message(client_fd, res_code, filename, buf_size, reply_buffer);
        if (result != 0) {
            LOG_ERROR("send message(): %s\n", strerror(errno));
            return NULL;
        }

        free(reply_buffer);
        free_message(message);

        // LOG_INFO(CLIENT "%s on [%s] done\n", client_fd - 5, msg_code_to_str(code), filename);

        switch (code) {
            case REQ_END: {
                int end = -1;
                write(pipe_fd, &end, sizeof(int));
                LOG_INFO(CLIENT "has ended the connection\n",
                            client_fd);

                // close(client_fd);
                break;
            }
            default: {
                write(pipe_fd, &client_fd, sizeof(int));
                break;
            }
        }
        
    }
    return NULL;
}

