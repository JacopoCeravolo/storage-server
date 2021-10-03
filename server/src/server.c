/**************************************************************************/
/* This example program provides code for a server application that uses     */
/* AF_UNIX address family                                                 */
/**************************************************************************/

/**************************************************************************/
/* Header files needed for this sample program                            */
/**************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
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

typedef enum _MODE {
   WRITE    = 13,
   NOT_YET  = 14,
} MODE;

void spawn_thread(long conn_fd, MODE mode);

void writeReq(void* args);

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

   /* Opening the socket */

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
   LOG_INFO("Awaiting connection.\n");

   /* Starting the connection */
      
   message_t *msg = malloc(sizeof(message_t));
   char      *buffer = malloc(MAX_PATH);
   /* pthread_attr_t  thread_attr;
   pthread_t       tid;
 */
   sd2 = accept(sd, (struct sockaddr*)NULL, NULL);
   if (sd2 < 0) {
     perror("accept() failed");
      return -1;
   }
   LOG_INFO("Connection accepted\n");

   /* while (TRUE)
   {
      sd2 = accept(sd, (struct sockaddr*)NULL, NULL);
      if (sd2 < 0) {
        perror("accept() failed");
         return -1;
      }

      LOG_INFO("Connection accepted\n");

      LOG_DEBUG("receiving message from client\n");
      if (recv_message(sd2, msg) != 0) {
         LOG_ERROR("recv_message(): %s\n", strerror(errno));
      }

      LOG_DEBUG("message received\n");

      
      printf(BOLDMAGENTA "\nREQUEST\n" RESET);
      printf(BOLD "\nMESSAGE HEADER:\n" RESET);
      printf("Code:      %s\n", msg_code_to_str(msg->header.code));
      printf("File:      %s\n",msg->header.filename);
      printf("Body Size: %ld\n", msg->header.msg_size);
      printf(BOLD "BODY:\n" RESET);
      printf("%s\n\n", (char*)msg->body);

      LOG_DEBUG("sending reply to client\n");
      memset(buffer, '0', MAX_PATH);
      strcpy(buffer, HANDSHAKE_MSG);
      set_message(msg, RES_SUCCESS, DEFAULT_SOCKET_PATH, strlen(buffer) + 1, buffer);

      if (send_message(sd2, msg) != 0) {
         LOG_ERROR("send_message(): %s\n", strerror(errno));
      }
      LOG_DEBUG("reply sent\n");

      LOG_DEBUG("receiving message from client\n");
      if (recv_message(sd2, msg) != 0) {
         LOG_ERROR("recv_message(): %s\n", strerror(errno));
      }

      LOG_DEBUG("message received\n");

      switch (msg->header.code) {
         case REQ_WRITE: 
      }
      
   } */
   

   do {

      LOG_DEBUG("receiving message from client\n");
      if (recv_message(sd2, msg) != 0) {
         LOG_ERROR("recv_message(): %s\n", strerror(errno));
      }

      LOG_DEBUG("message received\n");

      
      printf(BOLDMAGENTA "\nREQUEST\n" RESET);
      printf(BOLD "\nMESSAGE HEADER:\n" RESET);
      printf("Code:      %s\n", msg_code_to_str(msg->header.code));
      printf("File:      %s\n",msg->header.filename);
      printf("Body Size: %ld\n", msg->header.msg_size);
      printf(BOLD "BODY:\n" RESET);
      printf("%s\n\n", (char*)msg->body);

      LOG_DEBUG("sending reply to client\n");
      memset(buffer, '0', MAX_PATH);
      strcpy(buffer, HANDSHAKE_MSG);
      set_message(msg, RES_SUCCESS, DEFAULT_SOCKET_PATH, strlen(buffer) + 1, buffer);

      if (send_message(sd2, msg) != 0) {
         LOG_ERROR("send_message(): %s\n", strerror(errno));
      }
      LOG_DEBUG("reply sent\n");

      /* LOG_DEBUG("receiving message from client\n");
      if (recv_message(sd2, msg) != 0) {
         LOG_ERROR("recv_message(): %s\n", strerror(errno));
      }

      LOG_DEBUG("message received\n");

      
      printf(BOLDMAGENTA "\nREQUEST\n" RESET);
      printf(BOLD "\nMESSAGE HEADER:\n" RESET);
      printf("Code:      %s\n", msg_code_to_str(msg->header.code));
      printf("File:      %s\n",msg->header.filename);
      printf("Body Size: %ld\n", msg->header.msg_size);
      printf(BOLD "BODY:\n" RESET);
      printf("%s\n\n", (char*)msg->body);

      LOG_DEBUG("sending reply to client\n");
      memset(buffer, '0', MAX_PATH);
      strcpy(buffer, HANDSHAKE_MSG);
      set_message(msg, RES_SUCCESS, DEFAULT_SOCKET_PATH, strlen(buffer) + 1, buffer);

      if (send_message(sd2, msg) != 0) {
         LOG_ERROR("send_message(): %s\n", strerror(errno));
      }
      LOG_DEBUG("reply sent\n");
 */
   } while (TRUE);

   free(buffer);
   free(msg->body);
   free(msg);
   if (sd != -1)
      close(sd);

   if (sd2 != -1)
      close(sd2);

}

/* void spawn_thread(long conn_fd, MODE mode)
{
    pthread_attr_t  thread_attr;
    pthread_t       tid;

    if(pthread_attr_init(&thread_attr) != 0) {
        perror("pthread_attr_init");
        close(conn_fd);
        return;
    } 
    if(pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED) != 0) {
        pthread_attr_destroy(&thread_attr);
        perror("pthread_attr_setdetachstate");
        close(conn_fd);
        return;
    }       
    if(mode == WRITE) {
        if(pthread_create(&tid, &thread_attr, writeReq, (void *)conn_fd) != 0) {
        pthread_attr_destroy(&thread_attr);
        perror("pthread_create");
        close(conn_fd);
        return;
        }
    }
    
} */