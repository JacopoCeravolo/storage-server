#include "api/include/filestorage_api.h"

int writeFile(const char *pathname, const char *dirname)
{
    if (pathname == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Opening file from disk */
    if (DEBUG)
        LOG_DEBUG("opening file [%s] from disk\n", pathname);

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
    if (DEBUG)
        LOG_DEBUG("reading file [%s] contents\n", pathname);

    void* file_data;
    file_data = malloc(file_size);
    if (file_data == NULL) {
        errno = ENOMEM;
        fclose(file_ptr);
    }

    if (fread(file_data, 1, file_size, file_ptr) < file_size) {
        if (ferror(file_ptr)) {
            fclose(file_ptr);
            if (file_data != NULL) free(file_data);
        }
    }

    fclose(file_ptr);

    int result, flags = 0;
    SET_FLAG(flags, O_CREATE);
    SET_FLAG(flags, O_LOCK);

    if (openFile(pathname, flags) != 0) return -1;


    if (DEBUG)
        LOG_DEBUG("writing file [%s] to server\n", pathname);

    message_t *message;

    result = send_message(socket_fd, REQ_WRITE, pathname, file_size, file_data);
    if (result != 0) return -1;

    if (DEBUG)
      LOG_DEBUG("awaiting server response\n");

    message = recv_message(socket_fd);
    if (message == NULL) return -1;

    if (DEBUG)
        print_message(message);

    switch (message->code) {
        default: result = 0;
      // TODO: controllo risultato
    }

    free_message(message);
    return result;

}