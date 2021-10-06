#include "api/include/filestorage_api.h"

int 
readFile(const char* pathname, void** buf, size_t* size)
{
    /* Requesting read of file */
    LOG_DEBUG("requesting read of file [%s]\n", pathname);
    message_t *msg = malloc(sizeof(message_t));
    set_message(msg, REQ_READ, pathname, strlen(pathname) + 1, (char*)pathname);
    if (send_message(socket_fd, msg) != 0) {
      LOG_ERROR("send_message(): %s\n", strerror(errno));
      return -1;
    }
    /* Read server response */
    LOG_DEBUG("awaiting server response\n");

    if (recv_message(socket_fd, msg) != 0) {
      LOG_ERROR("recv_message(): %s\n", strerror(errno));
      return -1;
    }

    printf(BOLDMAGENTA "\nRESPONSE\n" RESET);
    printf(BOLD "\nMESSAGE HEADER:\n" RESET);
    printf("Code:      %s\n", msg_code_to_str(msg->header.code));
    printf("File:      %s\n",msg->header.filename);
    printf("Body Size: %ld\n", msg->header.msg_size);
    printf(BOLD "BODY:\n" RESET);
    printf("%s\n\n", (char*)msg->body);

    // TODO check response, if success copy in buffer
    *size = msg->header.msg_size;
    buf = &msg->body;

    free(msg->body);
    free(msg);

    return 0;
}
