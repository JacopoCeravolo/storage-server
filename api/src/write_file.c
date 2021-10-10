#include "api/include/filestorage_api.h"

int 
writeFile(const char* pathname, const char* dirname)
{
    if (pathname == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Opening file from disk */
    LOG_DEBUG("opening file [%s] from disk\n", pathname);
    FILE *file_ptr;
    file_ptr = fopen(pathname, "r");
    if (file_ptr == NULL) {
        errno = EIO;
        return -1;
    }

    
    /* Get file size */
    fseek(file_ptr, 0, SEEK_END);
    long file_size = ftell(file_ptr);
    fseek(file_ptr, 0, SEEK_SET);
    if (file_size == -1) {
        fclose(file_ptr);
        return -1;
    }

    /* Read file contents */
    LOG_DEBUG("reading file contents\n");
    char *file_data;
    file_data = malloc(file_size);
    if (file_data == NULL) {
        errno = ENOMEM;
        fclose(file_ptr);

    }

    if (fread(file_data, sizeof(char), file_size, file_ptr) < file_size) {
        if (ferror(file_ptr)) {
            fclose(file_ptr);
            if (file_data != NULL) free(file_data);
        }
    }

    fclose(file_ptr);

    if (openFile(pathname, O_CREATE) != -1) {
        /* Writes file to server */
        LOG_DEBUG("writing file [%s] to server\n", pathname);
        
        message_t *message;
    
        message = set_message(REQ_WRITE, pathname, file_size, file_data);
    
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
    
        free(file_data);
        free(message->body);
        free(message);
        return 0;
    }

    return -1;
}