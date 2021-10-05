#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>

#include "utils/include/logger.h"
#include "utils/include/utilities.h"
#include "utils/include/protocol.h"


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

   int    socket_fd=-1, conn_fd=-1;
   int    rc;
   struct sockaddr_un serveraddr;
   char *filelist[5] = {"file1", "file2", "file3", "file4", "file5"}; // fake file list

   /* Opening the socket */

   unlink(DEFAULT_SOCKET_PATH);

   socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
   if (socket_fd < 0) {
      perror("socket() failed");
      return -1;
   }

   memset(&serveraddr, 0, sizeof(serveraddr));
   serveraddr.sun_family = AF_UNIX;
   strcpy(serveraddr.sun_path, DEFAULT_SOCKET_PATH);

   rc = bind(socket_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
   if (rc < 0) {
      perror("bind() failed");
      return -1;
   }

   rc = listen(socket_fd, 10);
   if (rc< 0) {
      perror("listen() failed");
      return -1;
   }

   LOG_INFO("Awaiting connection.\n");

   char      *buffer = malloc(MAX_PATH);

   conn_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL);
   if (conn_fd < 0) {
      perror("accept() failed");
      return -1;
   }
   LOG_INFO("New connection\n");
   
   /* Starts accepting requests */
   int termina = FALSE;
   while (!termina)
   {
      message_t *msg = malloc(sizeof(message_t));
      set_message(msg, RES_UNKNOWN, "", 0, NULL);

      LOG_INFO("awaiting new request from client\n");
      if (recv_message(conn_fd, msg) != 0) {
         LOG_ERROR("recv_message(): %s\n", strerror(errno));
         return -1;
      }


      printf(BOLDMAGENTA "\nREQUEST\n" RESET);
      printf(BOLD "\nHEADER:\n" RESET);
      printf("Code:      %s\n", msg_code_to_str(msg->header.code));
      printf("File:      %s\n",msg->header.filename);
      printf("Body Size: %ld\n", msg->header.msg_size);
      printf(BOLD "BODY:\n" RESET);
      (msg->header.code == REQ_READ_N) ? printf("%d\n\n", *(int*)msg->body) : 
                                         printf("%s\n\n", (char*)msg->body);

      
      switch (msg->header.code) {
         case REQ_END: {
            termina = TRUE;
            LOG_INFO("Closing the connection\n");
            break;
         }
         /* Receiving handshake */
         case REQ_WELCOME: {

            /* Sending handhsake */
            memset(buffer, '0', MAX_PATH);
            strcpy(buffer, HANDSHAKE_MSG);
            set_message(msg, RES_SUCCESS, DEFAULT_SOCKET_PATH, strlen(buffer) + 1, buffer);
            if (send_message(conn_fd, msg) != 0) {
               LOG_ERROR("send_message(): %s\n", strerror(errno));
               return -1;
            }
            LOG_INFO("connection accepted\n"); 
            break;
         }

         /* Receiving write request */
         case REQ_WRITE: {
            LOG_INFO("writing request\n");
            char filename[MAX_PATH];
            strcpy(filename, msg->header.filename);
            memset(buffer, '0', MAX_PATH);
            strcpy(buffer, "WRITE REQUEST RESULT");
            set_message(msg, RES_SUCCESS, filename, strlen(buffer) + 1, buffer);
            if (send_message(conn_fd, msg) != 0) {
               LOG_ERROR("send_message(): %s\n", strerror(errno));
               return -1;
            }
            LOG_INFO("request completed\n");
            break;
         }

         /* Receiving read request */
         case REQ_READ: {
            LOG_INFO("reading request\n");
            char filename[MAX_PATH];
            strcpy(filename, msg->header.filename);
            memset(buffer, '0', MAX_PATH);
            strcpy(buffer, "READ REQUEST RESULT");
            set_message(msg, RES_SUCCESS, filename, strlen(buffer) + 1, buffer);
            if (send_message(conn_fd, msg) != 0) {
               LOG_ERROR("send_message(): %s\n", strerror(errno));
               return -1;
            }
            LOG_INFO("request completed\n");
            break;
         }

         /* Receiving read multiple files */
         case REQ_READ_N: {

            // Tested with temp fake file list
            LOG_INFO("received read request of %d files\n", *(int*)msg->body);
            char *to_send = malloc(MAX_PATH * 5);
            strcpy(to_send, filelist[0]);
            strcat(to_send, ":");
            for (int i = 1; i < 5; i++) {
               strcat(to_send, filelist[i]);
               strcat(to_send, ":");
            }
            set_message(msg, RES_UNKNOWN, "", strlen(to_send) + 1, to_send);
            if (send_message(conn_fd, msg) != 0) {
               LOG_ERROR("send_message(): %s\n", strerror(errno));
               return -1;
            }
            free(to_send);
            break;
         }
         default: {
            memset(buffer, '0', MAX_PATH);
            strcpy(buffer, "default message reply");
            set_message(msg, RES_SUCCESS, DEFAULT_SOCKET_PATH, strlen(buffer) + 1, buffer);
            if (send_message(conn_fd, msg) != 0) {
               LOG_ERROR("send_message(): %s\n", strerror(errno));
               return -1;
            }
            break;
         } 
      }
      free(msg->body);
      free(msg);
   }

   free(buffer);

   if (socket_fd != -1) close(socket_fd);
   if (conn_fd != -1) close(conn_fd);
   return 0;
}
