#include "client/include/option_parser.h"

#include "utils/include/logger.h"

const char*
option_to_str(option_code code)
{
    switch (code) {
        case OPTION_READ:   return "read files";
        case OPTION_WRITE:  return "write files";
        case OPTION_RREAD:  return "read n files";
        case OPTION_RWRITE: return "write directory";
        case OPTION_LOCK:   return "lock files";
        case OPTION_UNLOCK: return "unlock files";
        case OPTION_REMOVE: return "remove files";
        default:            return "unknown option";
    }
}

int
parse_options(int argc,  const char *argv[], const char *options_str)
{
    int         opt;
    option_t    new_option;

    while ((opt = getopt(argc, argv, options_str)) != -1) {
        switch (opt) {
            case 'h': {
                if (DEBUG) LOG_DEBUG("print helper option recognized\n");
                config->print_helper = true;
                return 0;
            }
            case 'f': {
                if (DEBUG) LOG_DEBUG("set socket option recognized\n");
                memset(config->socket_name, 0, MAX_SOCKET_PATH);
                strcpy(config->socket_name, optarg);
                if (DEBUG) LOG_DEBUG("socket name set to [%s]\n", config->socket_name);
                break;
            }
            case 'p': {
                if (DEBUG) LOG_DEBUG("set verbose option recognized\n");
                config->verbose = true;
                break;
            }
            case 't': {
                if (DEBUG) LOG_DEBUG("set time option recognized\n");
                config->wait_time = atoi(optarg); // check whether is number
                break;
            }
            case 'd': {
                if (DEBUG) LOG_DEBUG("set reading directory option recognized\n");
                memset(config->reading_dir, 0, MAX_PATH);
                strcpy(config->reading_dir, optarg);
                if (DEBUG) LOG_DEBUG("reading directory set to [%s]\n", config->reading_dir);
                break;
            }
            case 'D': {
                if (DEBUG) LOG_DEBUG("set expelled directory option recognized\n");
                memset(config->expelled_dir, 0, MAX_PATH);
                strcpy(config->expelled_dir, optarg);
                if (DEBUG) LOG_DEBUG("expelled directory set to [%s]\n", config->expelled_dir);
                break;
            }
            case 'r': {
                if (DEBUG) LOG_DEBUG("read multiple files option recognized\n");
                new_option.code = OPTION_READ;
                new_option.files_list = createQueue(MAX_PATH);
                if (new_option.files_list == NULL) {
                    LOG_FATAL("when allocating new argument list for option -%c\n", opt);
                    return -1;
                }

                char *filename;
                char *abs_name;
                filename = strtok(optarg, ",");
                if (filename == NULL) LOG_WARNING("could not read files for reading\n");
                while (filename != NULL) {
                    // abs_name = realpath(filename, NULL);
                    if (enqueue(new_option.files_list, filename) != 0) {
                        LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                    } 
                    // LOG_DEBUG("added file [%s] to arguments list\n", filename);
                    filename = strtok(NULL, ",");
                }
                if (enqueue(option_list, &new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'R': {
                if (DEBUG) LOG_DEBUG("read random files option recognized\n");
                new_option.code = OPTION_RREAD;
                new_option.how_many_files = atoi(optarg);
                if (enqueue(option_list, &new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'W': {
                if (DEBUG) LOG_DEBUG("write multiple files option recognized\n");
                new_option.code = OPTION_WRITE;
                new_option.files_list = createQueue(MAX_PATH);
                if (new_option.files_list == NULL) {
                    LOG_FATAL("when allocating new argument list for option -%c\n", opt);
                    return -1;
                }

                char *filename;
                char *abs_name;
                filename = strtok(optarg, ",");
                if (filename == NULL) LOG_WARNING("could not read files for writing\n");
                while (filename != NULL) {
                    // abs_name = realpath(filename, NULL);
                    if (enqueue(new_option.files_list, filename) != 0) {
                        LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                    } 
                    // if (DEBUG) LOG_DEBUG("added file [%s] to arguments list\n", filename);
                    filename = strtok(NULL, ",");
                }
                if (enqueue(option_list, &new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'l': {
                if (DEBUG) LOG_DEBUG("lock option recognized\n");
                new_option.code = OPTION_LOCK;
                new_option.files_list = createQueue(MAX_PATH);
                if (new_option.files_list == NULL) {
                    LOG_FATAL("when allocating new argument list for option -%c\n", opt);
                    return -1;
                }

                char *filename;
                char *abs_name;
                filename = strtok(optarg, ",");
                if (filename == NULL) LOG_WARNING("could not read files for locking\n");
                while (filename != NULL) {
                    // abs_name = realpath(filename, NULL);
                    if (enqueue(new_option.files_list, filename) != 0) {
                        LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                    } 
                    // if (DEBUG) LOG_DEBUG("added file [%s] to arguments list\n", filename);
                    filename = strtok(NULL, ",");
                }
                if (enqueue(option_list, &new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'u': {
                if (DEBUG) LOG_DEBUG("unlock option recognized\n");
                new_option.code = OPTION_UNLOCK;
                new_option.files_list = createQueue(MAX_PATH);
                if (new_option.files_list == NULL) return -1;
                char *filename;
                filename = strtok(optarg, ",");
                while (filename != NULL) {
                    if (enqueue(new_option.files_list, filename) != 0) {
                        LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                    }
                    filename = strtok(NULL, ",");
                }
                if (enqueue(option_list, &new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'c': {
                    if (DEBUG) LOG_DEBUG("remove option recognized\n");
                    new_option.code = OPTION_REMOVE;
                    new_option.files_list = createQueue(MAX_PATH);
                    if (new_option.files_list == NULL) return -1;
                    char *filename;
                    filename = strtok(optarg, ",");
                    while (filename != NULL) {
                            if (enqueue(new_option.files_list, filename) != 0) {
                                LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                            }
                        filename = strtok(NULL, ",");
                    }
                    if (enqueue(option_list, &new_option) != 0) {
                        LOG_WARNING("option -%c and its arguments were not added\n", opt);
                    }
                    break;
            }
            case '?': {
                LOG_ERROR("unrecognized option [ -%c ]\n", optopt);
                break;
            }
            case ':': {
                switch (optopt) {
                case 'R': {
                    if (DEBUG) LOG_DEBUG("read random files option recognized\n");
                    new_option.code = OPTION_RREAD;
                    new_option.how_many_files = 0;
                    if (enqueue(option_list, &new_option) != 0) {
                        LOG_WARNING("option -%c and its arguments were not added\n", opt);
                    }
                    break;
                }
                default:
                    LOG_ERROR("option [ -%c ] requires an argument\n", optopt);
                    break;
                }
            }
            default:
                break;

        }
    }
    return 0;
}
/* int
parse_options(queue_t *option_list, int argc,  const char *argv[], const char *options_str)
{
    if (option_list == NULL) {
        return -1;
    }

    int opt;
    option_t new_option;
    while ((opt = getopt(argc, argv, options_str)) != -1)
    {   
        switch (opt) {
            case 'w': {
                LOG_DEBUG("write directory option recognized\n");
                new_option.code = OPTION_RWRITE;
                
                char *dirname;
                dirname = strtok(optarg, ",");
                strcpy(new_option.w_arg.dirname, dirname);
                
                char *how_many;
                how_many = strtok(NULL, " ");
                LOG_DEBUG("%s\n", how_many);
                if ((how_many = strchr(how_many, 'n')) == NULL) {
                    LOG_ERROR("option -%c requires the second argument to be in the form 'n=0'\n", optopt);
                }
                LOG_DEBUG("%s\n", how_many);
                if ((how_many = strchr(how_many, '=')) == NULL) {
                    LOG_ERROR("option -%c requires the second argument to be in the form 'n=0'\n", optopt);
                }
                LOG_DEBUG("%s\n", how_many);
                if (isNumber((++how_many), &new_option.w_arg.how_many) != 0) {
                    // TODO? check if 2 then ERANGE
                    LOG_ERROR("option -%c requires the second argument to be in the form 'n=0'\n", optopt);
                }
                LOG_DEBUG("%d\n", new_option.w_arg.how_many);
                if (enqueue(option_list, &new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'R': {
                LOG_DEBUG("read multiple files option recognized\n");
                new_option.code = OPTION_RREAD;
                new_option.how_many_files = atoi(optarg);
                if (enqueue(option_list, &new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'l': {
                LOG_DEBUG("lock option recognized\n");
                new_option.code = OPTION_LOCK;
                new_option.filenames_list = createQueue(PATH_LEN);
                if (new_option.filenames_list == NULL) {
                    LOG_FATAL("when allocating new argument list for option -%c\n", opt);
                    return -1;
                }

                char *filename;
                char *abs_name;
                filename = strtok(optarg, ",");
                if (filename == NULL) LOG_WARNING("could not read files for locking\n");
                while (filename != NULL) {
                    // abs_name = realpath(filename, NULL);
                    if (enqueue(new_option.filenames_list, filename) != 0) {
                        LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                    } 
                    // LOG_DEBUG("added file [%s] to arguments list\n", filename);
                    filename = strtok(NULL, ",");
                }
                if (enqueue(option_list, &new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'u': {
                LOG_DEBUG("unlock option recognized\n");
                new_option.code = OPTION_UNLOCK;
                new_option.filenames_list = createQueue(PATH_LEN);
                if (new_option.filenames_list == NULL) return -1;
                char *filename;
                filename = strtok(optarg, ",");
                while (filename != NULL) {
                    if (enqueue(new_option.filenames_list, filename) != 0) {
                        LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                    }
                    filename = strtok(NULL, ",");
                }
                if (enqueue(option_list, &new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'c': {
                    LOG_DEBUG("remove option recognized\n");
                    new_option.code = OPTION_REMOVE;
                    new_option.filenames_list = createQueue(PATH_LEN);
                    if (new_option.filenames_list == NULL) return -1;
                    char *filename;
                    filename = strtok(optarg, ",");
                    while (filename != NULL) {
                            if (enqueue(new_option.filenames_list, filename) != 0) {
                                LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                            }
                        filename = strtok(NULL, ",");
                    }
                    if (enqueue(option_list, &new_option) != 0) {
                        LOG_WARNING("option -%c and its arguments were not added\n", opt);
                    }
                    break;
                }
            case '?': {
                LOG_ERROR("unrecognized option [ -%c ]\n", optopt);
                break;
            }
            default:
                break;
        }
    }
    return 0;   
}
 */