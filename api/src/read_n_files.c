#include "api/include/filestorage_api.h"

int readNFiles(int N, const char *dirname)
{
  /* Requesting read of N files */
  if (DEBUG)
    LOG_DEBUG("requesting read of %d files\n", N);
 
  int result;
  message_t *message;
  
  result = send_message(socket_fd, REQ_READ_N, "", sizeof(int), &N);
  if (result != 0) return -1;

  if (DEBUG)
    LOG_DEBUG("awaiting server response\n");

  message = recv_message(socket_fd);
  if (message == NULL) return -1;

  if (DEBUG)
    print_message(message);

  switch (message->code) {
    case RES_SUCCESS: break;
    default: {
      free_message(message);
      return -1;
    }
  }

  char *filelist = (char*)message->body;
  char *filename = strtok(filelist, ":");
  void* reading_buffer;
  size_t buffer_size;

  free_message(message);

  while (filename != NULL) {
    result = readFile(filename, &reading_buffer, &buffer_size);
    if (result != 0) {
      LOG_ERROR("could not read file [%s]\n", filename);
    }
    filename = strtok(NULL, ":");
  }

  return result;
}
