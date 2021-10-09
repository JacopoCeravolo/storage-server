#ifndef SERVER_CONFIG_H
#define SERVER_CONFIGH_H

#include "server/include/storage.h"
#include "utils/include/logger.h"

#define DISPATCHER  BOLD "[DISPATCHER] " RESET
#define WORKER      BOLD "[WORKER %d] " RESET
#define CLIENT      BOLDMAGENTA "client %d " RESET

#define HANDSHAKE_MSG   "CONNECTION ACCEPTED"

#define FALSE              0
#define TRUE               1

storage_t *storage;

#endif