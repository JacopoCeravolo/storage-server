#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "client/include/client_config.h"
#include "client/include/option_parser.h"

#include "api/include/filestorage_api.h"

#include "utils/include/logger.h"
#include "utils/include/queue.h"

void
print_help_msg();

int
execute_request(option_t opt);

queue_t     *option_list;
config_t    *config; 

int 
main(int argc, char const *argv[])
{
    /* Check if the program has at least one argument */
    if (argc < 2) {
        LOG_INFO("Client program requires at least one argument, rerun with -h for usage\n");
        exit(EXIT_FAILURE);
    }

    /* Sets configuration to default values */
     
    config = malloc(sizeof(config_t));
    config->client_id = 1;
    config->wait_time = 0;
    config->print_helper = false;
    config->verbose = false; 
    strcpy(config->socket_name, SAFE_SOCKET_PATH);
    strcpy(config->reading_dir, DIRECTORY_NOT_SET);
    strcpy(config->expelled_dir, DIRECTORY_NOT_SET);

    /* Initialize option queue  */
    option_list = createQueue(sizeof(option_t));
    if (option_list == NULL) {
        LOG_FATAL("option list not created. ERROR: %s\n", strerror(errno));
        return -1;
    }

    /* Parse options */
    if (parse_options(argc, argv, CLIENT_OPTIONS) != 0) {
        LOG_WARNING("option have not been parsed correctly, there might be unexpected results\n");
    }

    /* Check whether to print help and exit */
    if (config->print_helper == true) {
        if (DEBUG) LOG_DEBUG("About to print helper and exit\n");
        print_help_msg();
        destroyQueue(option_list);
        free(config);
        return 0;
    }

    /* Prints out parsed values */

    LOG_INFO(BOLD "CLIENT CONFIGURATION\n" RESET);
    LOG_INFO(BOLD "SOCKET:      " RESET "%s\n", config->socket_name);
    LOG_INFO(BOLD "VERBOSE:     " RESET "%s\n", (config->verbose) ? "true" : "false");
    LOG_INFO(BOLD "WAIT TIME:   " RESET "%d\n", config->wait_time);
    LOG_INFO(BOLD "READ DIR:    " RESET "%s\n", config->reading_dir);
    LOG_INFO(BOLD "TRASH DIR:   " RESET "%s\n", config->expelled_dir);

    
    printf("\n");
    // LOG_INFO(BOLD "PARSED OPTIONS\n" RESET);
    option_t opt;
    while (!isEmpty(option_list)) {
        dequeue(option_list, &opt);
        execute_request(opt);
    }

    destroyQueue(option_list);
    free(config);
    return 0;
}

int
execute_request(option_t opt)
{
    if (DEBUG) LOG_DEBUG("executing request " BOLD "%s\n" RESET, option_to_str(opt.code));
    switch (opt.code) {
        case OPTION_READ: {
            /* char *pathname = malloc(MAX_PATH);
            size_t unsued;
            while (!isEmpty(opt.files_list)) {
                dequeue(opt.files_list, pathname);
                if (readFile(pathname, NULL, &unsued) != 0) {
                    LOG_ERROR("could not read file [%s] from server.\n", pathname);
                } else {
                    LOG_INFO("successfully read file [%s] from server.\n", pathname);
                }
            }
            free(pathname); */
            destroyQueue(opt.files_list);
            break;
        }
        case OPTION_WRITE: {
            /* char *pathname = malloc(MAX_PATH);
            size_t unsued;
            char *abs_dirname = NULL;
            if (strcmp(config->expelled_dir, DIRECTORY_NOT_SET) != 0) {
                abs_dirname = malloc(MAX_PATH);
                strcpy(abs_dirname, config->expelled_dir);
                // TODO: get absolute path
            }
            if (abs_dirname != NULL) LOG_DEBUG("abs dir: %s\n", abs_dirname);
            while (!isEmpty(opt.files_list)) {
                dequeue(opt.files_list, pathname);
                if (writeFile(pathname, abs_dirname) != 0) {
                    LOG_ERROR("could not write file [%s] to server.\n", pathname);
                } else {
                    LOG_INFO("successfully written file [%s] to server.\n", pathname);
                }
            }
            free(pathname); */
            destroyQueue(opt.files_list);
            break;
        }
        case OPTION_RREAD: {
            
            break;
        }
        case OPTION_RWRITE: {
            
            break;
        }
        case OPTION_LOCK: {
            destroyQueue(opt.files_list);
            break;
        }
        case OPTION_UNLOCK: {
            destroyQueue(opt.files_list);
            break;
        }
        case OPTION_REMOVE: {
            destroyQueue(opt.files_list);
            break;
        }
    }
}

// Prints help message
void
print_help_msg()
{
    printf("\nThe client program allows to interact with the File Storage Server to send, receive, edit and delete files.\n" \
                                        "It behaves differently according to the options passed through the command line.\n" \
                                        "\nCurrently included options are:\n\n" \
                                        "    -h                        Prints this message\n" \
                                        "    -p                        Enables printing of infomation for each operation in the format:\n" \
                                        "                              OPT_TYPE      FILE      RESULT      N_BYTES\n" \
                                        "    -f sockname               Specifes the name of AF_UNIX socket to connect to\n" \
                                        "    -w dirname[,n=n_files]    Sends the contents of directory dirname to File Storage Server.\n" \
                                        "                              All subdirectories are visited recursively sending up to n_files.\n" \
                                        "                              If n=0 or n is unspecifed, all contents are sent to File Storage Server\n" \
                                        "    -W file1[,file2...]       Sends files specifed as arguments separated by commas to File Storage Server\n" \
                                        "    -D dirname                Specifies the directory dirname where to write files expelled by File Storage Server\n" \
                                        "                              in case of capacity misses. Option -D should be coupled with -w or -W, otherwise an error\n" \
                                        "                              message is print and all expelled files are trashed. If not specified all expelled files are trashed\n" \
                                        "    -r file1[,file2...]       Reads files specifed as arguments separated by commas from File Storage Server\n" \
                                        "    -R [n=n_files]            Reads n_files files from File Storage Server. If n=0 or unspecified reads all files in File Storage Server\n" \
                                        "    -d dirname                Specifies the directory dirname where to write files read from File Strage Server.\n" \
                                        "                              Option -d should be coupled with -r or -R, otherwise an error message is print and read files are not saved\n" \
                                        "    -t time                   Times in milleseconds to wait in between requests to File Storage Server\n" \
                                        "    -l file1[,file2...]       List of files to acquire mutual exclusion on\n" \
                                        "    -u file1[,file2...]       List of files to release mutual exclusion on\n" \
                                        "    -c file1[,file2...]       List of files to delete from File Storage Server\n");
}

