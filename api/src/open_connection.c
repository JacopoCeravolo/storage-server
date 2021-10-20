#include "api/include/filestorage_api.h"

int 
openConnection(const char* sockname, int msec, const struct timespec abstime)
{
   /* Initialize sockaddr_un and tries to connect */

   if (DEBUG) LOG_DEBUG("attempting to open a connection on socket [%s]\n", sockname);

   int res;
   struct sockaddr_un serveraddr;
   socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
   if (socket_fd < 0) {
      return -1;
   }
   memset(&serveraddr, 0, sizeof(serveraddr));
   serveraddr.sun_family = AF_UNIX;
   strcpy(serveraddr.sun_path, sockname);
   res = connect(socket_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)); // Use SUN_LEN
   if (res < 0) {
      return -1;
   }

   /* Send handsake message to server */

   message_t *handshake;
   char      *buffer = malloc(MAX_PATH);
   strcpy(buffer, HANDSHAKE_MSG);

   handshake = set_message(REQ_WELCOME, sockname, strlen(buffer) + 1, (void*)buffer);
   
   if (DEBUG) LOG_DEBUG("sending handshake message\n");
   if (send_message(socket_fd, handshake) != 0) {
      LOG_ERROR("send_message(): %s\n", strerror(errno));
      return -1;
   }

   /* Read handshake result */

   if (DEBUG) LOG_DEBUG("awaiting server handshake\n");
   if ((handshake = recv_message(socket_fd)) == NULL) {
      LOG_ERROR("recv_message(): %s\n", strerror(errno));
      return -1;
   }

   /* printf(BOLD "\nRESPONSE\n" RESET
            BOLD "Code: " RESET "%s\n"
            BOLD "File: " RESET "%s\n"
            BOLD "BODY\n" RESET "%s\n", 
            msg_code_to_str(handshake->header.code), 
            handshake->header.filename, (char*)handshake->body); */
    
   /* Cleanup */
   int result = (handshake->header.code == RES_SUCCESS) ? 0 : -1;

   free(buffer);
   free(handshake->body);
   free(handshake);
   return result;
}
