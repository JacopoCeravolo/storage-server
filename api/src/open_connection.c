#include "api/include/filestorage_api.h"

int 
openConnection(const char* sockname, int msec, const struct timespec abstime)
{
   /* Initialize sockaddr_un and tries to connect */

   LOG_DEBUG("attempting to open a connection on socket [%s]\n", sockname);
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
   
   LOG_DEBUG("sending handshake message\n");
   if (send_message(socket_fd, handshake) != 0) {
      LOG_ERROR("send_message(): %s\n", strerror(errno));
      return -1;
   }

   /* Read handshake result */

   LOG_DEBUG("awaiting server handshake\n");
   if ((handshake = recv_message(socket_fd)) == NULL) {
      LOG_ERROR("recv_message(): %s\n", strerror(errno));
      return -1;
   }

   printf(BOLDMAGENTA "\nHANDSHAKE\n" RESET);
   printf(BOLD "\nMESSAGE HEADER:\n" RESET);
   printf("Code:      %s\n", msg_code_to_str(handshake->header.code));
   printf("File:      %s\n",handshake->header.filename);
   printf("Body Size: %ld\n", handshake->header.msg_size);
   printf(BOLD "BODY:\n" RESET);
   printf("%s\n\n", (char*)handshake->body);
    
   /* Cleanup */
   
   free(buffer);
   free(handshake->body);
   free(handshake);
   return 0;
}
