#include "api/include/filestorage_api.h"

int 
closeConnection(const char* sockname)
{
    if (DEBUG) LOG_DEBUG("closing the connection on socket [%s]\n", sockname);
    message_t *message;
    char      *buffer = malloc(MAX_PATH);
    strcpy(buffer, CLOSING_MSG);

    message = set_message(REQ_END, sockname, strlen(buffer) + 1, buffer);
   
   printf(BOLD "\nREQUEST\n" RESET
            BOLD "Code: " RESET "%s\n"
            BOLD "File: " RESET "%s\n"
            BOLD "BODY\n" RESET "%s\n", 
            msg_code_to_str(message->header.code), 
            message->header.filename, (char*)message->body);
   
    if (send_message(socket_fd, message) != 0) {
        // LOG_ERROR("send_message(): %s\n", strerror(errno));
        return -1;
    }

    if (DEBUG) LOG_DEBUG("awaiting server response\n");

    if ((message = recv_message(socket_fd)) == NULL) {
      // LOG_ERROR("recv_message(): %s\n", strerror(errno));
      return -1;
    }

    printf(BOLD "\nRESPONSE\n" RESET
            BOLD "Code: " RESET "%s\n"
            BOLD "File: " RESET "%s\n"
            BOLD "BODY\n" RESET "%s\n", 
            msg_code_to_str(message->header.code), 
            message->header.filename, (char*)message->body);
    
    free(buffer);
    free(message->body);
    free(message);

    errno = 0;
    if (close(socket_fd) != 0) {
      return -1;
    }
    
    return 0;
}
