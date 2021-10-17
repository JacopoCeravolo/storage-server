#include "server/include/storage.h"
#include "utils/include/utilities.h"

int
open_file_handler(int client_id, storage_t *storage, char *pathname, int flags, void* reply_buffer, size_t *buf_size);

int 
write_file_handler(int client_id, storage_t *storage, char *pathname, size_t size, void* contents, void* reply_buffer, size_t *buf_size);

int
read_file_handler(int client_id, storage_t *storage, char *pathname, void* reply_buffer, size_t *buf_size);

list_t*
read_n_file_handler(storage_t *storage, int N, void* reply_buffer, size_t *buf_size);



