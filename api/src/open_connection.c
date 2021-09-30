#include "api/include/filestorage_api.h"

int 
openConnection(const char* sockname, int msec, const struct timespec abstime)
{
    int rc;
    struct sockaddr_un serveraddr;
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
       perror("socket() failed");
       return -1;
    }
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, sockname);
    rc = connect(socket_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)); // Use SUN_LEN
    if (rc < 0)
    {
        perror("connect() failed");
       return -1;
    }

    message_t *msg = malloc(sizeof(message_t));
    char      *buffer = malloc(MAX_PATH);
    strcpy(buffer, "Message\n");
    set_message(msg, RES_UNKNOWN, "file", strlen(buffer) + 1, buffer);

    send_message(socket_fd, msg);

    recv_message(socket_fd, msg);
    
    return 0;
}
