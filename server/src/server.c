/**************************************************************************/
/* This example program provides code for a server application that uses     */
/* AF_UNIX address family                                                 */
/**************************************************************************/

/**************************************************************************/
/* Header files needed for this sample program                            */
/**************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#include "utils/include/logger.h"
#include "utils/include/utilities.h"
#include "utils/include/protocol.h"

/**************************************************************************/
/* Constants used by this program                                         */
/**************************************************************************/

#define HANDSHAKE_MSG   "CONNECTION ACCEPTED"

#define FALSE              0
#define TRUE               1

void
cleanup()
{
   unlink(DEFAULT_SOCKET_PATH);
}
int 
main(int argc, char * const argv[])
{
   atexit(cleanup);

   int    sd=-1, sd2=-1;
   int    rc;
   // char   buffer[BUFFER_LENGTH];
   struct sockaddr_un serveraddr;



      unlink(DEFAULT_SOCKET_PATH);
      sd = socket(AF_UNIX, SOCK_STREAM, 0);
      if (sd < 0)
      {
         perror("socket() failed");
         return -1;
      }

      memset(&serveraddr, 0, sizeof(serveraddr));
      serveraddr.sun_family = AF_UNIX;
      strcpy(serveraddr.sun_path, DEFAULT_SOCKET_PATH);

      rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
      if (rc < 0)
      {
         perror("bind() failed");
         return -1;
      }

      rc = listen(sd, 10);
      if (rc< 0)
      {
         perror("listen() failed");
         return -1;
      }

      printf("> Ready for client connect().\n");

      message_t *msg = malloc(sizeof(message_t));
      char      *buffer = malloc(MAX_PATH);

   do {
      sd2 = accept(sd, NULL, NULL);
      if (sd2 < 0)
      {
         perror("accept() failed");
         break;
      }

      printf("> Connection accepted\n");

      recv_message(sd2, msg);

      printf(BOLD "\n\tRESPONSE\n");
      printf("\n\tMESSAGE HEADER:\n" RESET);
      printf("\tCode:      %s\n", msg_code_to_str(msg->header.code));
      printf("\tFile:      %s\n",msg->header.filename);
      printf("\tBody Size: %ld\n", msg->header.msg_size);
      printf(BOLD "\tBODY:\n" RESET);
      printf("\t%s\n\n", (char*)msg->body);
 

      memset(buffer, '0', MAX_PATH);
      strcpy(buffer, HANDSHAKE_MSG);

      set_message(msg, RES_SUCCESS, DEFAULT_SOCKET_PATH, strlen(buffer) + 1, buffer);
      
      send_message(sd2, msg);
      
      
      
      
   

   } while (TRUE);

   free(buffer);
   free(msg->body);
   free(msg);
   if (sd != -1)
      close(sd);

   if (sd2 != -1)
      close(sd2);

}