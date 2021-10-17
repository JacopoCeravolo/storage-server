#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>

#include "utils/include/logger.h"
#include "utils/include/utilities.h"
#include "utils/include/protocol.h"

#include "server/include/storage.h"

#define HANDSHAKE_MSG   "CONNECTION ACCEPTED"

#define FALSE              0
#define TRUE               1

void
cleanup()
{
   unlink(DEFAULT_SOCKET_PATH);
}


