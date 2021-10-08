#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>

#include "utils/include/logger.h"
#include "utils/include/utilities.h"
#include "utils/include/protocol.h"

#include "server/include/storage.h"
#include "server/include/server_config.h"
// #include "server/include/threadpool.h"
#include "server/include/worker.h"

void
cleanup()
{
   unlink(DEFAULT_SOCKET_PATH);
}

storage_t *storage = NULL;

int 
main(int argc, char * const argv[])
{
   atexit(cleanup);

   int    socket_fd=-1;
   int    rc;
   struct sockaddr_un serveraddr;

   if ((storage = create_storage(4096, 100)) == NULL) {
      LOG_FATAL("could not create storage, exiting..\n");
      return -1;
   }
   
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

   rc = listen(socket_fd, 50);
   if (rc< 0) {
      perror("listen() failed");
      return -1;
   }

   LOG_INFO("Server is now ready to accept connections.\n");
   
   /* pool_t *pool = create_pool(5, 50);
   if (pool == NULL) {
       LOG_INFO("could not create pool\n");
       return (void*)-1;
   } */
   
   lqueue_t *requests = malloc(sizeof(lqueue_t));

   requests->queue = createQueue(sizeof(long));
   requests->queue_size = 0;
   // Mutex and conidition var initialization
   if ((pthread_mutex_init(&(requests->lock), NULL) != 0) ||
	   (pthread_cond_init(&(requests->notify), NULL) != 0)) {
	   destroyQueue(requests->queue);
      free(requests);
	   return -1;
   }
   
   LOG_INFO(BOLD "[MASTER] " RESET "queue created\n");
   LOG_INFO(BOLD "[MASTER] " RESET "starting threads\n");

   pthread_t *workers = malloc(sizeof(pthread_t) * 5);
   for (int i = 0; i < 5; i++) {
      worker_arg_t *worker_args = malloc(sizeof(worker_args));
      worker_args->exit = 0;
      worker_args->worker_id = i;
      worker_args->requests = requests;
      if (pthread_create(&workers[i], NULL, worker_thread, (void*)worker_args) != 0) {
         LOG_FATAL("could not create thread\n");
         exit(EXIT_FAILURE);
      }
   }

   /* Starts accepting requests */

   fd_set set;  // insieme fd
   fd_set tmpset;
   FD_ZERO(&set);
   FD_ZERO(&tmpset);
   FD_SET(socket_fd, &set);
   
   int fdmax = (socket_fd > 0) ? socket_fd : 0;

   
   while (1) {
      tmpset = set;
	   if (select(fdmax+1, &tmpset, NULL, NULL, NULL) == -1) {
	       perror("select");
	       exit(-1);
	   }
      for(int i=0; i <= fdmax; i++) {

         if (FD_ISSET(i, &tmpset)) {

            long client_fd;
		      if (i == socket_fd && FD_ISSET(socket_fd, &set)) { // new client
               client_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL);
               if (client_fd < 0) {
                  perror("accept() failed");
                  return -1;
               }
               LOG_INFO(BOLD "[MASTER] " RESET "new connection from client\n");
               FD_SET(client_fd, &set);
               if (client_fd > fdmax) fdmax = client_fd;
            } else { // new request from already connected client
               // LOG_INFO(BOLD "[MASTER] " RESET "new request\n");
               
               LOCK_RETURN(&(requests->lock), -1);
               // LOG_INFO(BOLD "[MASTER] " RESET "enqueuing request\n");
               enqueue(requests->queue, (void*)&i);
               requests->queue_size++;

               pthread_cond_signal(&(requests->notify));
               
               UNLOCK_RETURN(&(requests->lock),-1);
            }
         }
      }
      /* pthread_t   master_id;
      pthread_create(&master_id, NULL, master_thread, (void*)socket_fd);
      pthread_join(master_id, NULL); */
   }

/*    while (!termina)
   {
      conn_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL);
      if (conn_fd < 0) {
         perror("accept() failed");
         return -1;
      }
      LOG_INFO("New connection\n");
      
      dispatcher_arg_t *dispatcher_arg = malloc(sizeof(dispatcher_arg_t));
      dispatcher_arg->client_fd = conn_fd;
      dispatcher_arg->termina = (long)&termina; */
      
      
      /* message_t *msg = malloc(sizeof(message_t));
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

      
      free(msg->body);
      free(msg); */
   

   if (socket_fd != -1) close(socket_fd);
   return 0;
}
