#include "api/include/filestorage_api.h"

int 
removeFile(const char* pathname)
{
    if (pathname == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Requesting remove of file */
    LOG_DEBUG("requesting remove of file [%s]\n", pathname);
    message_t *message;

    message = set_message(REQ_DELETE, pathname, strlen(pathname) + 1, (char*)pathname);

    if (send_message(socket_fd, message) != 0) {
      LOG_ERROR("send_message(): %s\n", strerror(errno));
      return -1;
    }

    /* Read server response */
    LOG_DEBUG("awaiting server response\n");
    
    if ((message = recv_message(socket_fd)) == NULL) {
      LOG_ERROR("recv_message(): %s\n", strerror(errno));
      return -1;
    }
    
    printf(BOLDMAGENTA "\nRESPONSE\n" RESET);
    printf(BOLD "\nMESSAGE HEADER:\n" RESET);
    printf("Code:      %s\n", msg_code_to_str(message->header.code));
    printf("File:      %s\n",message->header.filename);
    printf("Body Size: %ld\n", message->header.msg_size);
    printf(BOLD "BODY:\n" RESET);
    printf("%s\n\n", (char*)message->body);

    free(message->body);
    free(message);
    return 0;
}
