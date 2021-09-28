#include "filestorage_api.h"

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
    
    return 0;
}
