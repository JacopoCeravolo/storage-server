#include "api/include/filestorage_api.h"

int 
openConnection(const char* sockname, int msec, const struct timespec abstime)
{
    int rc;
    struct sockaddr_un serveraddr;
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
       return -1;
    }
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, sockname);
    rc = connect(socket_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)); // Use SUN_LEN
    if (rc < 0) {
       return -1;
    }

    message_t *msg = malloc(sizeof(message_t));
    char      *buffer = malloc(MAX_PATH);
    strcpy(buffer, HANDSHAKE_MSG);

    set_message(msg, REQ_WELCOME, sockname, strlen(buffer) + 1, buffer);

    send_message(socket_fd, msg);

    recv_message(socket_fd, msg);

    printf("\nRESPONSE\n");
    printf("\nMESSAGE HEADER:\n");
    printf("Code:      %s\n", msg_code_to_str(msg->header.code));
    printf("File:      %s\n",msg->header.filename);
    printf("Body Size: %ld\n", msg->header.msg_size);
    printf("BODY:\n");
    printf("%s\n\n", (char*)msg->body);
    
    free(buffer);
    free(msg->body);
    free(msg);
    return 0;
}
