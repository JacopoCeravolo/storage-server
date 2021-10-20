#include "api/include/filestorage_api.h"

int closeFile(const char *pathname)
{
  if (pathname == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (DEBUG)
    LOG_DEBUG("requesting close of file [%s]\n", pathname);

  int result;
  message_t *message;

  result = send_message(socket_fd, REQ_CLOSE, pathname, 0, NULL);
  if (result != 0) return -1;

  if (DEBUG)
    LOG_DEBUG("awaiting server response\n");

  message = recv_message(socket_fd);
  if (message == NULL) return -1;
  
  if (DEBUG)
    print_message(message);

  switch (message->code) {
    default: result = 0;
    // TODO: controllo risultato
  }

  free_message(message);
  return result;
}
