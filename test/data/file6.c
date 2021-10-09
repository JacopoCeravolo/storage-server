/* if (recv_message(conn_fd, msg) != 0) {
            LOG_ERROR("recv_message(): %s\n", strerror(errno));
            return (void*)-1;
        }

        Print request 
        LOG_INFO(BOLD "\n[WORKER %d]\n" RESET, worker_id);
        printf(BOLDMAGENTA "REQUEST\n" RESET);
        printf(BOLD "\nHEADER:\n" RESET);
        printf("Code:      %s\n", msg_code_to_str(msg->header.code));
        printf("File:      %s\n",msg->header.filename);
        printf("Body Size: %ld\n", msg->header.msg_size);
         printf(BOLD "BODY:\n" RESET);
        (msg->header.code == REQ_READ_N) ? printf("%d\n\n", *(int*)msg->body) : 
                                           printf("%s\n\n", (char*)msg->body); 

        switch (msg->header.code) {
            case REQ_END: {
               termina = 1;
               LOG_INFO(BOLD "[WORKER %d] " RESET"Closing the connection\n", worker_id);
               break;
            }
            Receiving handshake 
            case REQ_WELCOME: {   */