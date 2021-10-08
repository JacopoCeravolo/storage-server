#include "api/include/filestorage_api.h"

int 
closeConnection(const char* sockname)
{
    LOG_DEBUG("closing the connection on socket [%s]\n", sockname);
    message_t *message;
    char      *buffer = malloc(MAX_PATH);
    strcpy(buffer, CLOSING_MSG);

    message = set_message(REQ_END, sockname, strlen(buffer) + 1, buffer);
   
   
    if (send_message(socket_fd, message) != 0) {
        LOG_ERROR("send_message(): %s\n", strerror(errno));
        return -1;
    }

    free(buffer);
    free(message->body);
    free(message);
    return close(socket_fd);
}
