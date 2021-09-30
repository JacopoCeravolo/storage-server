#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "protocol.h"
#define sockname "/tmp/socket"

int main(int argc, char const *argv[])
{
    int rc;
    struct sockaddr_un serveraddr;
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket()");
       return -1;
    }
    // TODO: waiting for abstime
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, sockname);
    rc = connect(socket_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)); // Use SUN_LEN
    if (rc < 0) {
        perror("connect()");
       return -1;
    }

    
    message_t *msg = malloc(sizeof(message_t));
    
    char      *buffer = malloc(MAX_PATH);

    strcpy(buffer, "/********************************************************************/"
                     "/* The server uses the accept() function to accept an incoming      */"
                     "/* connection request.  The accept() call will block indefinitely   */"
                     "/* waiting for the incoming connection to arrive.                   */"
                     "/********************************************************************/\n"
                     "/********************************************************************/"
                     "/* The listen() function allows the server to accept incoming       */"
                     "/* client connections.  In this example, the backlog is set to 10.  */"
                     "/* This means that the system will queue 10 incoming connection     */"
                     "/* requests before the system starts rejecting the incoming         */"
                     "/* requests.                                                        */"
                     "/********************************************************************/\n");

    // double sent = 3.142392929393298839200030029;

    set_message(msg, REQ_OPEN, "file.txt", strlen(buffer) + 1, buffer);

    printf("\nREQUEST\n");
    printf("\nMESSAGE HEADER:\n");
    printf("Code:      %s\n", msg_code_to_str(msg->header.code));
    printf("File:      %s\n",msg->header.filename);
    printf("Body Size: %ld\n", msg->header.msg_size);
    printf("\nBODY\n");
    printf("%s\n", (char*)msg->body);

    send_message(socket_fd, msg);

    recv_message(socket_fd, msg);

    printf("\nRESPONSE\n");
    printf("\nMESSAGE HEADER:\n");
    printf("Code:      %s\n", msg_code_to_str(msg->header.code));
    printf("File:      %s\n",msg->header.filename);
    printf("Body Size: %ld\n", msg->header.msg_size);
    printf("\nBODY\n");
    printf("%s\n", (char*)msg->body);

    /* memcpy(msg->body, buf, msg->header.msg_size);

        
    printf("buffer is: %d\n", buffer);
    printf("body is: %c\n", (char)msg->body);
    write(socket_fd, &msg->header.code, sizeof(msg_code));
    write(socket_fd, msg->header.filename, MAX_PATH);
    write(socket_fd, &msg->header.msg_size, sizeof(size_t)); */

    /* Writes message body */

    /* // Change to while loop to check number of bytes 
    write(socket_fd, msg->body, msg->header.msg_size);
    // printf("writing %s\n", msg->body);
    

    read(socket_fd, &msg->header.code, sizeof(msg_code));
    read(socket_fd, msg->header.filename, MAX_PATH);
    read(socket_fd, &msg->header.msg_size, sizeof(size_t));

    //msg->body = malloc(msg->header.msg_size);
    
    read(socket_fd, msg->body, msg->header.msg_size); */
    // printf("receiving %s\n", msg->body);

    /* printf("MESSAGE HEADER:\n");
    printf("Code:      %d\n", msg->header.code);
    printf("File:      %s\n",msg->header.filename);
    printf("Body Size: %ld\n", msg->header.msg_size);
    printf("BODY\n");
    printf("%s\n", (char*)msg->body); */

    return 0;
}
