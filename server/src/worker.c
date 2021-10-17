#include <assert.h>
#include <unistd.h>


#include "server/include/worker.h"
#include "utils/include/logger.h"
#include "utils/include/protocol.h"
#include "server/include/server_config.h"
#include "server/include/request_handlers.h"


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
    
        int         code;
        char        filename[MAX_PATH];
        size_t      size;
        code = message->header.code;
        size = message->header.msg_size;
        strcpy(filename, message->header.filename);

        void *reply_buffer = malloc(MAX_BUFFER);
        size_t buf_size = MAX_BUFFER;

        int result;
        switch (code) {

            case REQ_OPEN: {
                result = open_file_handler(client_fd, storage, message->header.filename, 
                                            *(int*)message->body, reply_buffer, &buf_size);
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
            default: {
                    result = 0;
                    buf_size = strlen("SUCCESS") + 1;
                    memcpy(reply_buffer, "SUCCESS", buf_size);
                    break;
            }
        }

        int res_code = ((result == 0) ? RES_SUCCESS : RES_ERROR);
        message = set_message(res_code, filename, buf_size, reply_buffer);

        if (send_message(client_fd, message) != 0) {
           LOG_ERROR("send_message(): %s\n", strerror(errno));
           return NULL;
        }

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
        free(message->body);
        free(message);
        free(reply_buffer);
    }
    return NULL;
}

