#include "api/include/filestorage_api.h"

int 
readFile(const char* pathname, void** buf, size_t* size)
{
    /* Requesting read of file */
    LOG_DEBUG("requesting read of file [%s]\n", pathname);
    message_t *message;
    message = set_message(REQ_READ, pathname, strlen(pathname) + 1, (void*)pathname);
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

    // TODO check response, if success copy in buffer
    *size = message->header.msg_size;
    buf = &message->body;

    free(message->body);
    free(message);

    return 0;
}
