#include <assert.h>

#include "server/include/worker.h"
#include "utils/include/logger.h"
#include "utils/include/protocol.h"

void*
worker_thread(void* args)
{
    assert(args);

    int worker_id = ((worker_arg_t*)args)->worker_id;
    int exit = ((worker_arg_t*)args)->exit;
    lqueue_t *requests = ((worker_arg_t*)args)->requests;

    LOG_INFO(BOLD "[WORKER %d] " RESET "has started\n", worker_id);

    while (!exit) {

        long client_fd;
        LOCK_RETURN(&(requests->lock), NULL);
        while (requests->queue_size == 0) {
            pthread_cond_wait(&(requests->notify), &(requests->lock));
        }

        // CRITICAL SECTION
        dequeue(requests->queue, &client_fd);
        requests->queue_size--;
        UNLOCK_RETURN(&(requests->lock), NULL);
        // CRITICAL SECTION


        message_t *message;

        if ((message = recv_message(client_fd)) == NULL) {
            LOG_ERROR("recv_message(): %s\n", strerror(errno));
            return (void*)-1;
        }

        char filename[MAX_PATH];
        strcpy(filename, message->header.filename);

        LOG_INFO(BOLD "[WORKER %d] " RESET "new %s request of file %s from client %ld\n", 
                    worker_id, msg_code_to_str(message->header.code),
                    filename, client_fd % 4);


        char *buffer = calloc(MAX_BUFFER, 1);
        strcpy(buffer, "SUCCESS");
        message = set_message(RES_SUCCESS, filename, strlen(buffer) + 1, buffer);
        if (send_message(client_fd, message) != 0) {
           LOG_INFO("send_message(): %s\n", strerror(errno));
           return (void*)-1;
        }

        LOG_INFO(BOLD "[WORKER %d] " RESET "%s request of file %s from client %ld served\n", 
                    worker_id, msg_code_to_str(message->header.code),
                    message->header.filename, client_fd % 4);

        free(message->body);
        free(message);
        free(buffer);
    }
    return NULL;
}