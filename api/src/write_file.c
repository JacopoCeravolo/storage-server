#include "api/include/filestorage_api.h"

int 
writeFile(const char* pathname, const char* dirname)
{
    if (pathname == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Opening file from disk */
    if (DEBUG) if (DEBUG) LOG_DEBUG("opening file [%s] from disk\n", pathname);
    FILE *file_ptr;
    file_ptr = fopen(pathname, "rb");
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
    if (DEBUG) LOG_DEBUG("reading file contents\n");
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


    int flags = 0;
    SET_FLAG(flags, O_CREATE);
    SET_FLAG(flags, O_LOCK);


    if (openFile(pathname, flags) != -1) {

        LOG_INFO("%d %d -----> %d\n", O_CREATE, O_LOCK, O_CREATE|O_LOCK);
        /* Writes file to server */
        if (DEBUG) LOG_DEBUG("writing file [%s] to server\n", pathname);
        
        message_t *message;
    
        message = set_message(REQ_WRITE, pathname, file_size, file_data);
    
        printf(BOLD "\nREQUEST\n" RESET
            BOLD "Code: " RESET "%s\n"
            BOLD "File: " RESET "%s\n"
            BOLD "BODY\n" RESET "%s\n", 
            msg_code_to_str(message->header.code), 
            message->header.filename, (char*)message->body);

        if (send_message(socket_fd, message) != 0) {
          LOG_ERROR("send_message(): %s\n", strerror(errno));
          return -1;
        }
    
        /* Read server response */
        if (DEBUG) LOG_DEBUG("awaiting server response\n");
        
        if ((message = recv_message(socket_fd)) == NULL) {
          LOG_ERROR("recv_message(): %s\n", strerror(errno));
          return -1;
        }
        
        printf(BOLD "\nRESPONSE\n" RESET
            BOLD "Code: " RESET "%s\n"
            BOLD "File: " RESET "%s\n"
            BOLD "BODY\n" RESET "%s\n", 
            msg_code_to_str(message->header.code), 
            message->header.filename, (char*)message->body);

    
        int result = (message->header.code == RES_SUCCESS) ? 0 : -1;

        free(file_data);
        free(message->body);
        free(message);
        return result;
    }

    return -1;
}