#include "api/include/filestorage_api.h"

int openConnection(const char *sockname, int msec, const struct timespec abstime)
{
   /* Initialize sockaddr_un and tries to connect */

   if (sockname == NULL || msec < 0) {
      errno = EINVAL;
      return -1;
   }

   if (DEBUG)
      LOG_DEBUG("attempting to open a connection on socket [%s]\n", sockname);

   int result;
   struct sockaddr_un serveraddr;

   socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
   if (socket_fd < 0) return -1;
   memset(&serveraddr, 0, sizeof(serveraddr));
   serveraddr.sun_family = AF_UNIX;
   strcpy(serveraddr.sun_path, sockname);
   // strcpy(socket_name, sockname);

   result = connect(socket_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)); // Use SUN_LEN
   if (result < 0) return -1;
   // TODO: ritenta dopo m seconds


   message_t *handshake;
   char handshake_buffer[MAX_BUFFER];
   strncpy(handshake_buffer, HANDSHAKE_REQUEST, strlen(HANDSHAKE_REQUEST) + 1);

   result = send_message(socket_fd, REQ_WELCOME, sockname, strlen(HANDSHAKE_REQUEST) + 1, handshake_buffer);
   if (result != 0) return -1;


   if (DEBUG)
      LOG_DEBUG("awaiting server handshake\n");
   
   handshake = recv_message(socket_fd);
   if (handshake == NULL) return -1;

   if (DEBUG)
      print_message(handshake);

   switch (handshake->code) {
    // TODO: controllo risultato
      case RES_SUCCESS: result = 0; break;
      default: result = -1; break;
   }

   free_message(handshake);
   return result;
}
