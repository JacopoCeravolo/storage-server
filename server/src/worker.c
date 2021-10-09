#include <assert.h>
#include <unistd.h>


#include "server/include/worker.h"
#include "utils/include/logger.h"
#include "utils/include/protocol.h"
#include "server/include/server_config.h"

void*
worker_thread(void* args)
{
    assert(args);

    int worker_id = ((worker_arg_t*)args)->worker_id;
    int pipe_fd = ((worker_arg_t*)args)->pipe_fd;
    int exit = ((worker_arg_t*)args)->exit;
    lqueue_t *requests = ((worker_arg_t*)args)->requests;

    free(args);

    LOG_INFO(WORKER "has started\n", worker_id);

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
    
        msg_code    code;
        char        filename[MAX_PATH];
        size_t      size;
        code = message->header.code;
        size = message->header.msg_size;
        strcpy(filename, message->header.filename);

        char *reply_buffer = calloc(MAX_BUFFER, 1);

        switch (code) {

            case REQ_WRITE: {
                if (write_file(storage, message->header.filename, size, message->body) != 0) {
                    LOG_ERROR(WORKER "could not write file [%s] in storage, ERROR: %s\n", 
                                worker_id, filename, strerror(errno));
                    strcpy(reply_buffer, msg_code_to_str(RES_ERROR));
                    message = set_message(RES_ERROR, filename, strlen(reply_buffer) + 1, reply_buffer);
                } else {
                    strcpy(reply_buffer, "Write of file was successfull");
                    message = set_message(RES_SUCCESS, filename, strlen(reply_buffer) + 1, reply_buffer);
                }
                break;
            }

            case REQ_READ: {
                if ((reply_buffer = read_file(storage, message->header.filename)) == NULL) {
                    LOG_ERROR(WORKER "could not read file [%s] from storage, ERROR: %s\n", 
                                worker_id, filename, strerror(errno));
                    strcpy(reply_buffer, msg_code_to_str(RES_ERROR));
                    message = set_message(RES_ERROR, filename, strlen(reply_buffer) + 1, reply_buffer);
                }
                else {
                    message = set_message(RES_SUCCESS, filename, strlen(reply_buffer) + 1, reply_buffer);
                }
                break;
            }

            default: {
                    strcpy(reply_buffer, "SUCCESS");
                    message = set_message(RES_SUCCESS, filename, strlen(reply_buffer) + 1, reply_buffer);
                    break;
            }
        }

        /* LOG_INFO(BOLD "[WORKER %d] " RESET "new %s request of file %s from" BOLDMAGENTA " client %d\n" RESET, 
                    worker_id, msg_code_to_str(message->header.code),
                    filename, client_fd - 5); */

        if (send_message(client_fd, message) != 0) {
           LOG_ERROR("send_message(): %s\n", strerror(errno));
           return NULL;
           //if (errno != EPIPE) return (void*)-1;
        }

        LOG_INFO(WORKER "%s request of file %s from " CLIENT "served\n", 
                    worker_id, msg_code_to_str(code),
                    message->header.filename, client_fd - 5);

        switch (code) {
            case REQ_END: {
                int end = -1;
                write(pipe_fd, &end, sizeof(int));
                LOG_INFO(WORKER CLIENT "has ended the connection\n",
                            worker_id, client_fd - 5)
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