#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define TOTAL_SIZE              4096                            // 64 Kilobytes
#define BLOCK_SIZE              256                             // Predefined
#define DISK_BLOCKS             (int) (TOTAL_SIZE/BLOCK_SIZE)   // No. of blocks = total size / block size                         
#define NO_OF_FILES             20                              // Predefined
#define NO_OF_INODES            20                              // Predefined according to NO_OF_FILES  
#define NO_OF_DIRECT_PTR        10
#define NO_OF_SINGLE_IND_PTR    1
#define NO_OF_DOUBLE_IND_PTR    1                               
#define NO_OF_INODE_PTR         NO_OF_DIRECT_PTR +  \
                                NO_OF_SINGLE_IND_PTR + \
                                NO_OF_DOUBLE_IND_PTR 


typedef struct inode /* inode structure */
{
    int filesize;                   // file size
    int pointer[NO_OF_INODE_PTR];   // pointers to data block
    int *indirects;
    int n_indirects;
} inode;

typedef struct file_to_inode_mapping /* file to inode map structure */
{
    char filename[32];              // file name
    int inode_num;                  // inode number
} file_to_inode_mapping;

typedef struct super_block /* super block structure */
{
    int current_size;
    int no_of_occupied_blocks;
    int total_no_of_available_blocks; 
    int first_free_file_pos;
    bool inode_freelist[NO_OF_INODES];    // to check which inode no is free to assign to file
    bool datablock_freelist[DISK_BLOCKS]; //to check which data block is free to allocate to file
} super_block;

typedef struct block 
{
    bool    occupied;
    char    data[BLOCK_SIZE];
    size_t  size;
} block;


// Utilities
int find_first_free_inode(int);
int find_first_free_block(int);
int find_file_pos(char*);

int block_write(block*, char*, int);
int block_read(block*, char*, int);
void print_super_block();
void print_file(char*);
void list_files();
int check_storage_capacity(size_t);
