#include "api/include/filestorage_api.h"

int
openFile(const char* pathname, int flags)
{
   
    if (pathname == NULL || flags < 0) {
      errno = EINVAL;
      return -1;
    }

    message_t *request;
    request = set_message(REQ_OPEN, pathname, sizeof(int), &flags);

    errno = 0;
    if (send_message(socket_fd, request) != 0) {
      return -1;
    }

    // LOG_INFO("open_file(): awaiting server response\n");

    errno = 0;
    if ((request = recv_message(socket_fd)) == NULL) {
      return -1;
    }

    int result = (request->header.code == RES_SUCCESS) ? 0 : -1;

    if (result != 0) {
      memset(error_buffer, 0, 1024);
      memcpy(error_buffer, request->body, request->header.msg_size);
    }
    
    free(request->body);
    free(request);
    
    return result;
}