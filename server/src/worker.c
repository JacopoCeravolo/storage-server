#include <assert.h>
#include <unistd.h>


#include "server/include/worker.h"
#include "utils/include/logger.h"
#include "utils/include/protocol.h"

void*
worker_thread(void* args)
{
    assert(args);

    int worker_id = ((worker_arg_t*)args)->worker_id;
    int pipe_fd = ((worker_arg_t*)args)->pipe_fd;
    int exit = ((worker_arg_t*)args)->exit;
    lqueue_t *requests = ((worker_arg_t*)args)->requests;

    LOG_INFO(BOLD "[WORKER %d] " RESET "has started\n", worker_id);

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

        char filename[MAX_PATH];
        msg_code code = message->header.code;
        strcpy(filename, message->header.filename);

        /* LOG_INFO(BOLD "[WORKER %d] " RESET "new %s request of file %s from" BOLDMAGENTA " client %d\n" RESET, 
                    worker_id, msg_code_to_str(message->header.code),
                    filename, client_fd - 5); */


        char *buffer = calloc(MAX_BUFFER, 1);
        strcpy(buffer, "SUCCESS");
        message = set_message(RES_SUCCESS, filename, strlen(buffer) + 1, buffer);
        if (send_message(client_fd, message) != 0) {
           LOG_ERROR("send_message(): %s\n", strerror(errno));
           return NULL;
           //if (errno != EPIPE) return (void*)-1;
        }

        LOG_INFO(BOLD "[WORKER %d] " RESET "%s request of file %s from" BOLDMAGENTA " client %d" RESET " served\n", 
                    worker_id, msg_code_to_str(code),
                    message->header.filename, client_fd - 5);

        switch (code) {
            case REQ_END: {
                int end = -1;
                write(pipe_fd, &end, sizeof(int));
                LOG_INFO(BOLD "[WORKER %d] " RESET BOLDMAGENTA "client %d " RESET "has ended the connection\n",
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
        free(buffer);
    }
    return NULL;
}