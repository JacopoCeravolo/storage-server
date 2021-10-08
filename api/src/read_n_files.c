#include "api/include/filestorage_api.h"

int 
readNFiles(int N, const char* dirname)
{
    /* Requesting read of N files */
    LOG_DEBUG("requesting read of %d files\n", N);
    message_t *message;;
    message = set_message(REQ_READ_N, "", sizeof(int), &N);
    if (send_message(socket_fd, message) != 0) {
      LOG_ERROR("send_message(): %s\n", strerror(errno));
      return -1;
    }
    /* Read server response */
    LOG_DEBUG("awaiting server response\n");
    if ((message = recv_message(socket_fd)) == NULL) {
      LOG_ERROR("recv_message(): %s\n", strerror(errno));
      return -1;
    }
    printf(BOLDMAGENTA "\nRESPONSE\n" RESET);
    printf(BOLD "\nMESSAGE HEADER:\n" RESET);
    printf("Code:      %s\n", msg_code_to_str(message->header.code));
    printf("File:      %s\n",message->header.filename);
    printf("Body Size: %ld\n", message->header.msg_size);
    printf(BOLD "BODY:\n" RESET);
    printf("%s\n\n", (char*)message->body);

    char *filelist = (char*)message->body;
    char *filename = strtok(filelist, ":");

    char *reading_buffer = NULL;
    size_t size;

    while (filename != NULL) {
        if (readFile(filename, (void**)reading_buffer, &size) != 0) return -1;
        filename = strtok(NULL, ":");
    }

    free(message->body);
    free(message);
    return 0;
}
