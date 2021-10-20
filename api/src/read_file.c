#include "api/include/filestorage_api.h"

int readFile(const char *pathname, void **buf, size_t *size)
{
  // int result, flags = 0;

  /* SET_FLAG(flags, O_READ);
  if (openFile(pathname, flags) == -1) {
    LOG_ERROR("could not open file\n");
    return -1;
  } */
  if (pathname == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (DEBUG)
    LOG_DEBUG("requesting read of file [%s]\n", pathname);

  int result;
  message_t *message;

  result = send_message(socket_fd, REQ_READ, pathname, 0, NULL);
  if (result != 0) return -1;

  if (DEBUG)
    LOG_DEBUG("awaiting server response\n");

  message = recv_message(socket_fd);
  if (message == NULL) return -1;

  if (DEBUG)
    print_message(message);

  switch (message->code) {
    case RES_SUCCESS: {
      void* tmp_buf = malloc(message->size);
      memcpy(tmp_buf, message->body, message->size);
      *size = message->size;
      *buf = tmp_buf;
      result = 0; 
      break;
    }
    default: {
      memset(error_buffer, 0, 1024);
      memcpy(error_buffer, message->body, message->size);
      result = -1;
      break;
    }
  }

  free_message(message);
  return result;
}
