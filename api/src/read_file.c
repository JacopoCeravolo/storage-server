#include "api/include/filestorage_api.h"

int 
readFile(const char* pathname, void** buf, size_t* size)
{
  /* OPENS THE FILE */

  int flags = 0;
  SET_FLAG(flags, O_READ);
  if (openFile(pathname, flags) == -1) {
    LOG_ERROR("could not open file\n");
    return -1;
  }
    /* Requesting read of file */
    if (DEBUG) LOG_DEBUG("requesting read of file [%s]\n", pathname);
    message_t *message;

    message = set_message(REQ_READ, pathname, strlen(pathname) + 1, (void*)pathname);

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

    // TODO check response, if success copy in buffer
    *size = message->header.msg_size;
    buf = &message->body;

    int result = (message->header.code == RES_SUCCESS) ? 0 : -1;

    free(message->body);
    free(message);
    return result;
}
