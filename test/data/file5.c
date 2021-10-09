/*  LOG_INFO("New connection\n");
      
      dispatcher_arg_t *dispatcher_arg = malloc(sizeof(dispatcher_arg_t));
      dispatcher_arg->client_fd = conn_fd;
      dispatcher_arg->termina = (long)&termina; 
      
      
      message_t *msg = malloc(sizeof(message_t));
      set_message(msg, RES_UNKNOWN, "", 0, NULL);

      LOG_INFO("awaiting new request from client\n");
      if (recv_message(conn_fd, msg) != 0) {
         LOG_ERROR("recv_message(): %s\n", strerror(errno));
         return -1;
      }


      printf(BOLDMAGENTA "\nREQUEST\n" RESET);
      printf(BOLD "\nHEADER:\n" RESET);
      printf("Code:      %s\n", msg_code_to_str(msg->header.code));
      printf("File:      %s\n",msg->header.filename);
      printf("Body Size: %ld\n", msg->header.msg_size);
      printf(BOLD "BODY:\n" RESET);
      (msg->header.code == REQ_READ_N) ? printf("%d\n\n", *(int*)msg->body) : 
                                         printf("%s\n\n", (char*)msg->body);

      
      free(msg->body);
      free(msg); */