#include "api/include/filestorage_api.h"

int 
removeFile(const char* pathname)
{
    if (pathname == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Requesting remove of file */
    if (DEBUG) LOG_DEBUG("requesting remove of file [%s]\n", pathname);
    message_t *message;

    message = set_message(REQ_DELETE, pathname, strlen(pathname) + 1, (char*)pathname);

    printf(BOLD "\nREQUEST\n" RESET
            BOLD "Code: " RESET "%s\n"
            BOLD "File: " RESET "%s\n"
            BOLD "BODY\n" RESET "%s\n", 
            msg_code_to_str(message->header.code), 
            message->header.filename, (char*)message->body);


    if (send_message(socket_fd, message) != 0) {
      LOG_ERROR("send_message(): %s\n", strerror(errno));
      return -1;
    }

    /* Read server response */
    if (DEBUG) LOG_DEBUG("awaiting server response\n");
    
    if ((message = recv_message(socket_fd)) == NULL) {
      LOG_ERROR("recv_message(): %s\n", strerror(errno));
      return -1;
    }
    
    printf(BOLD "\nRESPONSE\n" RESET
            BOLD "Code: " RESET "%s\n"
            BOLD "File: " RESET "%s\n"
            BOLD "BODY\n" RESET "%s\n", 
            msg_code_to_str(message->header.code), 
            message->header.filename, (char*)message->body);


    int result = (message->header.code == RES_SUCCESS) ? 0 : -1;

    free(message->body);
    free(message);
    return result;
}
