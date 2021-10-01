#include "api/include/filestorage_api.h"

int 
closeConnection(const char* sockname)
{
    LOG_DEBUG("closing the connection on socket [%s]\n", sockname);
    close(socket_fd);
    return 0;
}
