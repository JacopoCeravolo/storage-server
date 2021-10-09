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
#include <fcntl.h>

#include "utils/include/logger.h"
#include "utils/include/utilities.h"
#include "utils/include/protocol.h"

#include "server/include/storage.h"
#include "server/include/server_config.h"
#include "server/include/worker.h"

void
cleanup()
{
   unlink(DEFAULT_SOCKET_PATH);

}

int termina = 0;
storage_t *storage = NULL;
FILE *logfile;

void int_handler(int dummy) {
   printf("\nHandling SIGNIT exit\n");
   if (storage_dump(storage, logfile) != 0) {
      LOG_ERROR("could not write to logfile\n");
   }
   exit(0);
}

int 
main(int argc, char * const argv[])
{
   atexit(cleanup);
   signal(SIGPIPE, SIG_IGN);
   signal(SIGINT, int_handler);

   /* Open logfile */
   logfile = fopen("logs/server-log.txt", "w+");

   /* Initilize storage */

   if ((storage = create_storage(4096, 100)) == NULL) {
      LOG_FATAL("could not create storage, exiting..\n");
      return -1;
   }
   LOG_INFO(DISPATCHER "storage initialized\n");
   
   /* Opening the socket */

   int    socket_fd=-1;
   int    rc;
   struct sockaddr_un serveraddr;

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

   LOG_INFO(DISPATCHER "socket ready\n");

   /* Sets pipes and FD_SET */

   int    mw_pipe[2];   // master-worker pipe
   fd_set set;          // set of opened file descriptors
   fd_set rdset;        // set of descriptors ready on read


   FD_ZERO(&set);
   FD_ZERO(&rdset);

   if (pipe(mw_pipe) != 0) {
      perror("pipe()");
      return -1;
   }
   /* fcntl(mw_pipe[0], F_SETNOSIGPIPE);
   fcntl(mw_pipe[1], F_SETNOSIGPIPE); */

   // signal(SIGPIPE, SIG_IGN);
   
   FD_SET(socket_fd, &set);
   FD_SET(mw_pipe[0], &set);

   int fd_max = 0;
   if (socket_fd > fd_max) fd_max = socket_fd;
   if (mw_pipe[0] > fd_max) fd_max = mw_pipe[0];
   /* Initialize bounded queue and starts threads */

   LOG_INFO(DISPATCHER "pipe and file descriptors ready\n");

   lqueue_t *requests = malloc(sizeof(lqueue_t));

   requests->queue = createQueue(sizeof(int));
   requests->queue_size = 0;
   // Mutex and conidition var initialization
   if ((pthread_mutex_init(&(requests->lock), NULL) != 0) ||
	   (pthread_cond_init(&(requests->notify), NULL) != 0)) {
	   destroyQueue(requests->queue);
      free(requests);
	   return -1;
   }

   LOG_INFO(DISPATCHER "starting threads\n");

   pthread_t *workers = malloc(sizeof(pthread_t) * 5);

   for (int id = 0; id < 5; id++) {
      worker_arg_t *worker_args = malloc(sizeof(worker_arg_t));
      worker_args->exit = 0;
      worker_args->worker_id = id;
      worker_args->pipe_fd = mw_pipe[1];
      worker_args->requests = requests;
      if (pthread_create(&workers[id], NULL, worker_thread, (void*)worker_args) != 0) {
         LOG_FATAL("could not create thread\n");
         exit(EXIT_FAILURE);
      }
   }

   /* Starts accepting requests */

   LOG_INFO(DISPATCHER "ready to accept requests\n");
   while (termina == 0) {
      rdset = set;
	   if (select(fd_max+1, &rdset, NULL, NULL, NULL) == -1 && errno != EINTR) {
	       perror("select");
	       exit(-1);
	   }
      
      for(int fd = 0; fd <= fd_max; fd++) {
         // printf("Examining fd: %d\n", fd);
         if (FD_ISSET(fd, &rdset) && (fd != mw_pipe[0])) {
            // printf("fd %d is set ", fd);
            int client_fd;
		      if (fd == socket_fd && FD_ISSET(socket_fd, &set)) { // new client
               // printf("and its new socket connection\n");
               client_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL);
               if (client_fd < 0) {
                  perror("accept() failed");
                  return -1;
               }
               LOG_INFO(DISPATCHER "accepted new connection from " CLIENT "\n", client_fd - 5);
               // LOG_INFO(BOLD "[MASTER] " RESET "new connection from client with fd %d\n", client_fd);
               // printf("adding fd %d (new client) to set\n", client_fd);
               FD_SET(client_fd, &set);
               if (client_fd > fd_max) fd_max = client_fd;

            } else { // new request from already connected client
               //LOG_INFO(BOLD "[MASTER] " RESET "new request from client with fd %d\n", fd);
               // printf("and its a new request from already connected client\n");
               LOCK_RETURN(&(requests->lock), -1);
               LOG_INFO(DISPATCHER "dispatching request of " CLIENT "\n", fd - 5);
               // LOG_INFO(BOLD "[MASTER] " RESET "enqueuing request\n");
               // printf("fd %d added to queue\n", fd);
               enqueue(requests->queue, (void*)&fd);
               requests->queue_size++;

               FD_CLR(fd, &set);
               // printf("fd %d removed from set\n", fd);
               if (fd == fd_max) {
                  fd_max--;
               }
               pthread_cond_signal(&(requests->notify));
               
               UNLOCK_RETURN(&(requests->lock),-1);
            }
         }
         if (FD_ISSET(fd, &rdset) && FD_ISSET(mw_pipe[0], &rdset)){
			   // printf("fd is set and is the mw_pipe\n");
            int new_fd;
            // printf("reading from pipe\n");
			   if( (read(mw_pipe[0], &new_fd, sizeof(int))) == -1 ){
			   	perror("read_pipe");
			   	return -1;
			   }
			   else{
               // printf("read successfull\n");
			   	if( new_fd != -1 ){ // reinserisco il fd tra quelli da ascoltare
                  // printf("added fd %d back to the set\n", new_fd);
			   		FD_SET(new_fd, &set);
			         if( new_fd > fd_max ) fd_max = new_fd;
			   	}
			   }
		   }
         // printf("No news on fd %d\n", fd);
      }

   }
   
   if (socket_fd != -1) close(socket_fd);
   return 0;
}
