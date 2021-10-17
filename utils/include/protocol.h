#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include "utils/include/utilities.h"

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

typedef struct _header_t {
    msg_code    code;
    char        filename[MAX_PATH];
    size_t      msg_size;
} header_t;

typedef struct _message_t {
    header_t    header;
    void*       body;
} message_t;

void
set_header(header_t *header, msg_code code, const char *filename, size_t msg_sz);

message_t*
set_message(msg_code code, const char *filename, size_t size, void* body);

int
send_message(int conn_fd, message_t *msg);

message_t*
recv_message(int conn_fd);

const char*
msg_code_to_str(msg_code code);

#endif