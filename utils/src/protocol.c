#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "protocol.h"

void
set_header(header_t *header, msg_code code, const char *filename, size_t msg_sz)
{
    header->code = code;
    memset(header->filename, 0, MAX_PATH);
    strncpy(header->filename, filename, MAX_PATH);
    header->msg_size = msg_sz;
}

void
set_message(message_t *msg, msg_code code, const char *filename, size_t size, void* body)
{
    msg->header.code = code;
    memset(msg->header.filename, '0', MAX_PATH);
    strcpy(msg->header.filename, filename);
    msg->header.msg_size = size;

    msg->body = malloc(msg->header.msg_size);
    memcpy(msg->body, body, msg->header.msg_size);
}


void
send_message(int conn_fd, message_t *msg)
{
    /* Writes header */

    write(conn_fd, &msg->header.code, sizeof(msg_code));
    write(conn_fd, msg->header.filename, MAX_PATH);
    write(conn_fd, &msg->header.msg_size, sizeof(size_t));

    /* Writes message body */

    // Change to while loop to check number of bytes 
    write(conn_fd, msg->body, msg->header.msg_size);
    // printf("writing %s\n", msg->body);
}

void
recv_message(int conn_fd, message_t *msg)
{
    size_t old_size = msg->header.msg_size;

    read(conn_fd, &msg->header.code, sizeof(msg_code));
    read(conn_fd, msg->header.filename, MAX_PATH);
    read(conn_fd, &msg->header.msg_size, sizeof(size_t));

    if (old_size < msg->header.msg_size) {
        msg->body = realloc(msg->body, msg->header.msg_size);
    }
    
    read(conn_fd, msg->body, msg->header.msg_size);
    // printf("receiving %s\n", msg->body);
}

const char*
msg_code_to_str(msg_code code)
{
    switch (code) {
        case RES_SUCCESS:   return "SUCCESS";
        case RES_WELCOME:   return "WELCOME";
        case REQ_WELCOME:   return "WELCOME";
        case REQ_OPEN:      return "OPEN";
        default:            return "MSG_CODE";
    }
}