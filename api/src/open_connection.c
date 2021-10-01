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

   message_t *msg = malloc(sizeof(message_t));
   char      *buffer = malloc(MAX_PATH);
   strcpy(buffer, HANDSHAKE_MSG);

   set_message(msg, REQ_WELCOME, sockname, strlen(buffer) + 1, buffer);
   
   LOG_DEBUG("sending handshake message\n");
   send_message(socket_fd, msg);

   /* Read handshake result */
   LOG_DEBUG("receiving server response\n");
   recv_message(socket_fd, msg);

   printf(BOLD "\n\tRESPONSE\n");
   printf("\n\tMESSAGE HEADER:\n" RESET);
   printf("\tCode:      %s\n", msg_code_to_str(msg->header.code));
   printf("\tFile:      %s\n",msg->header.filename);
   printf("\tBody Size: %ld\n", msg->header.msg_size);
   printf(BOLD "\tBODY:\n" RESET);
   printf("\t%s\n\n", (char*)msg->body);
    
   /* Cleanup */
   
   free(buffer);
   free(msg->body);
   free(msg);
   return 0;
}
