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
#include "utils/include/protocol.h"

/**************************************************************************/
/* Constants used by this program                                         */
/**************************************************************************/
#define SERVER_PATH     "/tmp/socket"
#define BUFFER_LENGTH    250
#define FALSE              0

void
cleanup()
{
   unlink(SERVER_PATH);
}
int 
main(int argc, char * const argv[])
{
   atexit(cleanup);

   int    sd=-1, sd2=-1;
   int    rc, length;
   // char   buffer[BUFFER_LENGTH];
   struct sockaddr_un serveraddr;


   do
   {

      unlink(SERVER_PATH);
      sd = socket(AF_UNIX, SOCK_STREAM, 0);
      if (sd < 0)
      {
         perror("socket() failed");
         break;
      }

      memset(&serveraddr, 0, sizeof(serveraddr));
      serveraddr.sun_family = AF_UNIX;
      strcpy(serveraddr.sun_path, SERVER_PATH);

      rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
      if (rc < 0)
      {
         perror("bind() failed");
         break;
      }

      rc = listen(sd, 10);
      if (rc< 0)
      {
         perror("listen() failed");
         break;
      }

      printf("Ready for client connect().\n");


      sd2 = accept(sd, NULL, NULL);
      if (sd2 < 0)
      {
         perror("accept() failed");
         break;
      }

      printf("connection accepted\n");



      message_t *msg = malloc(sizeof(message_t));

      recv_message(sd2, msg);

      printf("\nREQUEST\n");
      printf("\nMESSAGE HEADER:\n");
      printf("Code:      %s\n", msg_code_to_str(msg->header.code));
      printf("File:      %s\n",msg->header.filename);
      printf("Body Size: %ld\n", msg->header.msg_size);
      printf("\nBODY\n");
      printf("%s\n", (char*)msg->body);

      char      *buffer = malloc(MAX_PATH);

      strcpy(buffer, "Short string\n");

      set_message(msg, RES_SUCCESS, "file.txt", strlen(buffer) + 1, buffer);

      printf("\nRESPONSE\n");
      printf("\nMESSAGE HEADER:\n");
      printf("Code:      %s\n", msg_code_to_str(msg->header.code));
      printf("File:      %s\n",msg->header.filename);
      printf("Body Size: %ld\n", msg->header.msg_size);
      printf("\nBODY\n");
      printf("%s\n", (char*)msg->body);
      
      send_message(sd2, msg);
      

   

   } while (FALSE);


   if (sd != -1)
      close(sd);

   if (sd2 != -1)
      close(sd2);

}