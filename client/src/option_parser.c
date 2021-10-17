#include "client/include/option_parser.h"

#include "utils/include/logger.h"

#include <stdio.h>

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

void
print_option(void* e, FILE *f)
{
    option_t *opt = (option_t*)e;
    fprintf(f, "\nOption: %s\n", option_to_str(opt->code));
    switch (opt->code) {
        case OPTION_RREAD: { fprintf(f, "%d\n", opt->how_many_files); break; }
        case OPTION_RWRITE: {fprintf(f, "%s %d\n", opt->w_arg.dir, opt->w_arg.num); break; }
        default: {
            while (!list_is_empty(opt->files_list)) {
                fprintf(f, "-");
                void *path;
                list_remove_head(opt->files_list, &path);
                fprintf(f, "%s\n", (char*)path);
            }
            fprintf(f, "\n");
            break;
        }
    }
}

void
free_option(void* e)
{
    if (e != NULL) {
        option_t *opt = (option_t*)e;

        switch (opt->code) {
            case OPTION_RREAD: break;
            case OPTION_RWRITE: break;
            default: if (opt->files_list != NULL) list_destroy(opt->files_list); break;
        }

        free(opt);
    }

}

int
parse_options(int argc,  char * const argv[], const char *options_str)
{
    int         opt;

    while ((opt = getopt(argc, argv, options_str)) != -1) {
        option_t    *new_option = malloc(sizeof(option_t));
        switch (opt) {
            case 'h': {
                if (DEBUG) LOG_DEBUG("print helper option recognized\n");
                config->print_helper = true;
                return 0;
            }
            case 'f': {
                if (DEBUG) LOG_DEBUG("set socket option recognized\n");
                memset(config->socket_name, 0, MAX_PATH);
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
                new_option->code = OPTION_READ;
                new_option->files_list = list_create(NULL, NULL, NULL);
                if (new_option->files_list == NULL) {
                    LOG_FATAL("when allocating new argument list for option -%c\n", opt);
                    return -1;
                }

                char *filename;
                filename = strtok(optarg, ",");
                if (filename == NULL) LOG_WARNING("could not read files for reading\n");
                while (filename != NULL) {
                    if (list_insert_tail(new_option->files_list, (void*)filename) != 0) {
                        LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                    } 
                    // LOG_DEBUG("added file [%s] to arguments list\n", filename);
                    filename = strtok(NULL, ",");
                }
                if (list_insert_tail(option_list, (void*)new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'R': {
                if (DEBUG) LOG_DEBUG("read random files option recognized\n");
                new_option->code = OPTION_RREAD;
                new_option->how_many_files = atoi(optarg);
                if (list_insert_tail(option_list, (void*)new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'W': {
                if (DEBUG) LOG_DEBUG("write multiple files option recognized\n");
                new_option->code = OPTION_WRITE;
                new_option->files_list = list_create(NULL, NULL, NULL);
                if (new_option->files_list == NULL) {
                    LOG_FATAL("when allocating new argument list for option -%c\n", opt);
                    return -1;
                }

                char *filename;
                filename = strtok(optarg, ",");
                if (filename == NULL) LOG_WARNING("could not read files for writing\n");
                while (filename != NULL) {
                    if (list_insert_tail(new_option->files_list, (void*)filename) != 0) {
                        LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                    } 
                    // if (DEBUG) LOG_DEBUG("added file [%s] to arguments list\n", filename);
                    filename = strtok(NULL, ",");
                }
                if (list_insert_tail(option_list, (void*)new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'l': {
                if (DEBUG) LOG_DEBUG("lock option recognized\n");
                new_option->code = OPTION_LOCK;
                new_option->files_list = list_create(NULL, NULL, NULL);
                if (new_option->files_list == NULL) {
                    LOG_FATAL("when allocating new argument list for option -%c\n", opt);
                    return -1;
                }

                char *filename;
                filename = strtok(optarg, ",");
                if (filename == NULL) LOG_WARNING("could not read files for locking\n");
                while (filename != NULL) {
                    if (list_insert_tail(new_option->files_list, (void*)filename) != 0) {
                        LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                    } 
                    // if (DEBUG) LOG_DEBUG("added file [%s] to arguments list\n", filename);
                    filename = strtok(NULL, ",");
                }
                if (list_insert_tail(option_list, (void*)new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'u': {
                if (DEBUG) LOG_DEBUG("unlock option recognized\n");
                new_option->code = OPTION_UNLOCK;
                new_option->files_list = list_create(NULL, NULL, NULL);
                if (new_option->files_list == NULL) return -1;
                char *filename;
                filename = strtok(optarg, ",");
                while (filename != NULL) {
                    if (list_insert_tail(new_option->files_list, (void*)filename) != 0) {
                        LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                    }
                    filename = strtok(NULL, ",");
                }
                if (list_insert_tail(option_list, (void*)new_option) != 0) {
                    LOG_WARNING("option -%c and its arguments were not added\n", opt);
                }
                break;
            }
            case 'c': {
                    if (DEBUG) LOG_DEBUG("remove option recognized\n");
                    new_option->code = OPTION_REMOVE;
                    new_option->files_list = list_create(NULL, NULL, NULL);
                    if (new_option->files_list == NULL) return -1;
                    char *filename;
                    filename = strtok(optarg, ",");
                    while (filename != NULL) {
                            if (list_insert_tail(new_option->files_list, (void*)filename) != 0) {
                                LOG_WARNING("file [%s] not added, it won't be sent to server\n", filename);
                            }
                        filename = strtok(NULL, ",");
                    }
                    if (list_insert_tail(option_list, (void*)new_option) != 0) {
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
                    new_option->code = OPTION_RREAD;
                    new_option->how_many_files = 0;
                    if (list_insert_tail(option_list, &new_option) != 0) {
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
        //free(new_option);
    }
    return 0;
}