#include "api/include/filestorage_api.h"

int closeConnection(const char *sockname)
{
  if (sockname == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (DEBUG)
    LOG_DEBUG("closing the connection on socket [%s]\n", sockname);
  
  int result;
  message_t *message;

  char message_buffer[MAX_BUFFER];
  strncpy(message_buffer, CLOSING_REQUEST, strlen(CLOSING_REQUEST) + 1);

  result = send_message(socket_fd, REQ_END, sockname, 0, NULL);
  if (result != 0) return -1;

  if (DEBUG)
    LOG_DEBUG("awaiting server response\n");

  message = recv_message(socket_fd);
  if (message == NULL) return -1;
  
  if (DEBUG)
    print_message(message);

  free_message(message);


  return close(socket_fd);
  
}
