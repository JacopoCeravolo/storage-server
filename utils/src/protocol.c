#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "protocol.h"

int
send_message(int conn_fd, msg_code code, const char *filename, size_t size, void* body)
{
    int res;
    SYSCALL_RETURN(res, write(conn_fd, (void*)&code, sizeof(msg_code)), 
                "write()", res);
    //fprintf(stderr, "sent code\n");
    SYSCALL_RETURN(res, write(conn_fd, (void*)filename, sizeof(char) * MAX_PATH), 
                "write()", res);
    //fprintf(stderr, "sent name\n");
    SYSCALL_RETURN(res, write(conn_fd, (void*)&size, sizeof(size_t)), 
                "write()", res);
    //fprintf(stderr, "sent size\n");
    if (size != 0) {
        SYSCALL_RETURN(res, write(conn_fd, body, size), 
                "write()", res);
        //fprintf(stderr, "sent body\n");
    }

    return 0;
    
}

message_t*
recv_message(int conn_fd)
{
    message_t *message;

    message = malloc(sizeof(message_t));
    if (message == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    // Set message to safe values
    message->code = RES_UNKNOWN;
    memset(message->filename, '\0', MAX_PATH);
    message->size = 0;
    message->body = NULL;

    // Receives header
    int res;
    SYSCALL_GOTO(res, read(conn_fd, (void*)&message->code, sizeof(msg_code)),
                "read()", recv_message_exit);
    //fprintf(stderr, "got code\n");            
    SYSCALL_GOTO(res, read(conn_fd, (void*)message->filename, sizeof(char) * MAX_PATH),
                "read()", recv_message_exit);
    //fprintf(stderr, "got name\n");
    SYSCALL_GOTO(res, read(conn_fd, (void*)&message->size, sizeof(size_t)),
                "read()", recv_message_exit);
    //fprintf(stderr, "got size\n");

    // Allocates memory for message body if necessary
    if (message->size != 0) {
        message->body = malloc(message->size);
        if (message->body == NULL) {
            errno = ENOMEM;
            goto recv_message_exit;
        }
        SYSCALL_GOTO(res, read(conn_fd, message->body, message->size),
                "read()", recv_message_exit);
        //fprintf(stderr, "got body\n");
    }

    return message;

recv_message_exit:
    if (message->body) free(message->body);
    if (message) free(message);
    return NULL;
}


void
free_message(message_t *message)
{
    if (message->body) free(message->body);
    if (message) free(message);
}

const char*
msg_code_to_str(msg_code code)
{
    switch (code) {
        case RES_ERROR:     return "ERROR";
        case RES_UNKNOWN:   return "UNKNOWN";
        case RES_SUCCESS:   return "SUCCESS";
        case RES_MAXCONN:   return "MAX CONNECTION REACHED";
        case RES_BADRQST:   return "BAD REQUEST";
        case RES_WELCOME:   return "WELCOME";
        case REQ_WELCOME:   return "WELCOME";
        case REQ_OPEN:      return "OPEN FILE";
        case REQ_READ:      return "READ FILE";
        case REQ_READ_N:    return "READ N FILES";
        case REQ_WRITE:     return "WRITE FILE";
        case REQ_APPEND:    return "APPEND TO FILE";
        case REQ_LOCK:      return "LOCK FILE";
        case REQ_UNLOCK:    return "UNLOCK FILE";
        case REQ_CLOSE:     return "CLOSE FILE";
        case REQ_DELETE:    return "DELETE FILE";
        case REQ_END:       return "CLOSE CONNECTION";
        default:            return "MSG_CODE";
    }
}

void
print_message(message_t *message)
{
    printf("\n*************************\n");
    printf("* Message\n");
    printf("* Code: %s\n", msg_code_to_str(message->code));
    printf("* File: %s\n", message->filename);
    printf("* Size: %lu\n", message->size);
    /* if (message->body != NULL) {
        printf("* Body: ");
        if ((message->code == REQ_OPEN) || (message->code == REQ_READ_N)) {
            printf("%s\n", (char*)message->body);
        } else {
            printf("%d\n", *(int*)message->body);
        }
    } */
    printf("*************************\n");
}