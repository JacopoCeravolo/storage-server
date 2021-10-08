#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
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

message_t*
set_message(msg_code code, const char *filename, size_t size, void* body)
{
    message_t *msg = malloc(sizeof(message_t));
    msg->header.code = code;
    memset(msg->header.filename, '0', MAX_PATH);
    strcpy(msg->header.filename, filename);
    msg->header.msg_size = size;

    msg->body = malloc(msg->header.msg_size);
    memcpy(msg->body, body, msg->header.msg_size);
    return msg;
}


int
send_message(int conn_fd, message_t *msg)
{
    /* Writes header */

    if (write(conn_fd, &msg->header.code, sizeof(msg_code)) != sizeof(msg_code)) {
        printf("when writing message code\n");
        return -1;
    }
    if (write(conn_fd, msg->header.filename, MAX_PATH) != MAX_PATH) {
        printf("when writing message filename\n");
        return -1;
    }
    if (write(conn_fd, &msg->header.msg_size, sizeof(size_t)) != sizeof(size_t)) {
        printf("when writing message size\n");
        return -1;
    }

    /* Writes message body */

    // Change to while loop to check number of bytes 
    if (write(conn_fd, msg->body, msg->header.msg_size) != msg->header.msg_size) {
        printf("when writing message body\n");
        return -1;
    }
    // printf("writing %s\n", msg->body);
    return 0;
}

message_t*
recv_message(int conn_fd)
{
    message_t *msg = malloc(sizeof(message_t));
    int res;

    if ((res = read(conn_fd, &msg->header.code, sizeof(msg_code))) != sizeof(msg_code)) {
        if (res == -1) {
            printf("when reading message code\n");
            return NULL;
        }
    }
    if ((res = read(conn_fd, msg->header.filename, MAX_PATH)) != MAX_PATH) {
        if (res == -1) {
            printf("when reading message filename\n");
            return NULL;
        }
    }
    if ((res = read(conn_fd, &msg->header.msg_size, sizeof(size_t))) != sizeof(size_t)) {
        if (res == -1) {
            printf("when reading message size\n");
            return NULL;
        }
    }

    msg->body = malloc(msg->header.msg_size);
    if ((res = read(conn_fd, msg->body, msg->header.msg_size)) != msg->header.msg_size) {
        if (res == -1) {
            printf("when reading message body\n");
            return NULL;
        }
    }
    
    return msg;
}

const char*
msg_code_to_str(msg_code code)
{
    switch (code) {
        case RES_ERROR:     return "ERROR";
        case RES_UNKNOWN:   return "UNKNOWN";
        case RES_SUCCESS:   return "SUCCESS";
        case RES_MAXCONN:   return "MAX CONNECTION REACHED";
        case RES_BADRQST:   return "BAD REQUEST";
        case RES_WELCOME:   return "WELCOME";
        case REQ_WELCOME:   return "WELCOME";
        case REQ_OPEN:      return "OPEN FILE";
        case REQ_READ:      return "READ FILE";
        case REQ_READ_N:    return "READ N FILES";
        case REQ_WRITE:     return "WRITE FILE";
        case REQ_APPEND:    return "APPEND TO FILE";
        case REQ_LOCK:      return "LOCK FILE";
        case REQ_UNLOCK:    return "UNLOCK FILE";
        case REQ_CLOSE:     return "CLOSE FILE";
        case REQ_DELETE:    return "DELETE FILE";
        case REQ_END:       return "CLOSE CONNECTION";
        default:            return "MSG_CODE";
    }
}