#include "api/include/filestorage_api.h"

int openFile(const char *pathname, int flags)
{
  if (pathname == NULL || flags < 0) {
    errno = EINVAL;
    return -1;
  }

  if (DEBUG)
    LOG_DEBUG("attempting to open file [%s]\n", pathname);

  int result;
  message_t *message;
  
  result = send_message(socket_fd, REQ_OPEN, pathname, sizeof(int), &flags);
  if (result != 0) return -1;

  if (DEBUG)
    LOG_DEBUG("awaiting server response\n");

  message = recv_message(socket_fd);
  if (message == NULL) return -1;

  if (DEBUG)
    print_message(message);

  switch (message->code) {
    case RES_SUCCESS: result = 0; break;
    default: {
      result = -1;
      memset(error_buffer, 0, 1024);
      memcpy(error_buffer, message->body, message->size);
      break;
    }
  }

  free_message(message);
  return result;
}