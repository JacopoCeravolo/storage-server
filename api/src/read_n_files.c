#include "api/include/filestorage_api.h"

int 
readNFiles(int N, const char* dirname)
{
    /* Requesting read of N files */
    LOG_DEBUG("requesting read of %d files\n", N);
    message_t *msg = malloc(sizeof(message_t));
    set_message(msg, REQ_READ_N, "", sizeof(int), &N);
    if (send_message(socket_fd, msg) != 0) {
      LOG_ERROR("send_message(): %s\n", strerror(errno));
      return -1;
    }
    /* Read server response */
    LOG_DEBUG("awaiting server response\n");
    if (recv_message(socket_fd, msg) != 0) {
      LOG_ERROR("recv_message(): %s\n", strerror(errno));
      return -1;
    }
    printf(BOLDMAGENTA "\nRESPONSE\n" RESET);
    printf(BOLD "\nMESSAGE HEADER:\n" RESET);
    printf("Code:      %s\n", msg_code_to_str(msg->header.code));
    printf("File:      %s\n",msg->header.filename);
    printf("Body Size: %ld\n", msg->header.msg_size);
    printf(BOLD "BODY:\n" RESET);
    printf("%s\n\n", (char*)msg->body);

    char *filelist = (char*)msg->body;
    char *filename = strtok(filelist, ":");

    char *reading_buffer = NULL;
    size_t size;

    while (filename != NULL) {
        if (readFile(filename, (void**)reading_buffer, &size) != 0) return -1;
        filename = strtok(NULL, ":");
    }

    free(msg->body);
    free(msg);
    return 0;
}
