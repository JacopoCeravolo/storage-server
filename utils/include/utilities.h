#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef DEFAULT_SOCKET_PATH
#define DEFAULT_SOCKET_PATH     "/tmp/filestorage.sk"
#endif

#ifndef MAX_PATH    
#define MAX_PATH    1024
#endif

#ifndef MAX_BUFFER
#define MAX_BUFFER  4096
#endif

#ifndef MAX_OWNERS
#define MAX_OWNERS 10
#endif

#ifndef O_CREATE
#define O_CREATE  0
#endif

#ifndef DEBUG
#define DEBUG   false
#endif

#define DISPATCHER  BOLD "[DISPATCHER] " RESET
#define WORKER      BOLD "[WORKER %d] " RESET
#define CLIENT      BOLD "(client %d) " RESET

#define LOCK(l)      if (pthread_mutex_lock(l)!=0)        { \
    fprintf(stderr, "ERRORE FATALE lock\n");		    \
    pthread_exit((void*)EXIT_FAILURE);			    \
  }   
#define LOCK_RETURN(l, r)  if (pthread_mutex_lock(l)!=0)        {	\
    fprintf(stderr, "ERRORE FATALE lock\n");				\
    return r;								\
  }   

#define UNLOCK(l)    if (pthread_mutex_unlock(l)!=0)      {	    \
    fprintf(stderr, "ERRORE FATALE unlock\n");			    \
    pthread_exit((void*)EXIT_FAILURE);				    \
  }
#define UNLOCK_RETURN(l,r)    if (pthread_mutex_unlock(l)!=0)      {	\
    fprintf(stderr, "ERRORE FATALE unlock\n");				\
    return r;								\
  }
#define WAIT(c,l)    if (pthread_cond_wait(c,l)!=0)       {	    \
    fprintf(stderr, "ERRORE FATALE wait\n");			    \
    pthread_exit((void*)EXIT_FAILURE);				    \
  }
/* ATTENZIONE: t e' un tempo assoluto! */
#define TWAIT(c,l,t) {							\
    int r=0;								\
    if ((r=pthread_cond_timedwait(c,l,t))!=0 && r!=ETIMEDOUT) {		\
      fprintf(stderr, "ERRORE FATALE timed wait\n");			\
      pthread_exit((void*)EXIT_FAILURE);				\
    }									\
  }
#define SIGNAL(c)    if (pthread_cond_signal(c)!=0)       {		\
    fprintf(stderr, "ERRORE FATALE signal\n");				\
    pthread_exit((void*)EXIT_FAILURE);					\
  }
#define BCAST(c)     if (pthread_cond_broadcast(c)!=0)    {		\
    fprintf(stderr, "ERRORE FATALE broadcast\n");			\
    pthread_exit((void*)EXIT_FAILURE);					\
  }
static inline int TRYLOCK(pthread_mutex_t* l) {
  int r=0;		
  if ((r=pthread_mutex_trylock(l))!=0 && r!=EBUSY) {		    
    fprintf(stderr, "ERRORE FATALE unlock\n");		    
    pthread_exit((void*)EXIT_FAILURE);			    
  }								    
  return r;	
}

#endif