#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include "utils/include/utilities.h"

#define HANDSHAKE_REQUEST   "REQUESTING CONNECTION"
#define CLOSING_REQUEST     "CLOSING CONNECTION"

typedef enum {
    
    /* SERVER --> CLIENT */

    /* SERVER RESPONES */
    RES_ERROR               = -1,
    RES_UNKNOWN             = 0,
    RES_SUCCESS             = 1,
    RES_MAXCONN             = 2,
    RES_BADRQST             = 3, 

    /* HANDSHAKE */
    RES_WELCOME             = 10,


    /* CLIENT --> SERVER */

    /* HANDSHAKE */
    REQ_WELCOME             = 11,

    /* CLIENT REQUESTS */
    REQ_OPEN                = 12,
    REQ_READ                = 13,
    REQ_READ_N              = 14,
    REQ_WRITE               = 15,
    REQ_APPEND              = 16,
    REQ_LOCK                = 17,
    REQ_UNLOCK              = 18,
    REQ_CLOSE               = 19,
    REQ_DELETE              = 20,
    REQ_END                 = 21,

} msg_code;

typedef struct _message_t {
    msg_code    code;
    char        filename[MAX_PATH];
    size_t      size;
    void*       body;
} message_t;

int
send_message(int conn_fd, msg_code code, const char *filename, size_t size, void* body);

message_t*
recv_message(int conn_fd);

void
free_message(message_t *message);

const char*
msg_code_to_str(msg_code code);

void
print_message(message_t *message);

#endif