#include "api/include/filestorage_api.h"

int 
closeConnection(const char* sockname)
{
    close(socket_fd);
    return 0;
}
