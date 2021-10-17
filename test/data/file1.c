

/* #define BUF_SIZE 4096

int main(int argc, char const *argv[])

#define BUF_SIZE 4096

void handle_create()
{
    FILE *ifp;
    char *buffer;
    char name[32];
    buffer = calloc(BUF_SIZE, 1);

    scanf("%s", name);
    if ((ifp = fopen(name, "r+")) == NULL) {
        print_func(__func__, PRNT_ERROR, "could not open file");
    }

    fread(buffer, 1, BUF_SIZE, ifp);
    if (create_file(name, buffer) != 0) {
        print_func(__func__, PRNT_ERROR, "could not create file");
    } else print_func(__func__, PRNT_SUCCESS, "file correctly created");
}

void handle_print()
{
    char name[32];
    scanf("%s", name);
    print_file(name);
}

int main(int argc, char const *argv[])
{
    if (create_disk() == 0) {
        print_super_block();
    }
   
    char cmd[32];

    while (1) {
        printf(BOLDWHITE ">>> " RESET);
        scanf("%s", cmd);
        if (strcmp("add", cmd) == 0) handle_create();
        else if (strcmp("print_sb", cmd) == 0) print_super_block();
        else if (strcmp("print", cmd) == 0) handle_print();
        else if (strcmp("list", cmd) == 0) list_files();
        else if (strcmp("q", cmd) == 0 || strcmp("quit", cmd) == 0) break;
    }
}

#define BUF_SIZE 4096

void handle_create()
{
    FILE *ifp;
    char *buffer;
    char name[32];
    buffer = calloc(BUF_SIZE, 1);

    scanf("%s", name);
    if ((ifp = fopen(name, "r+")) == NULL) {
        print_func(__func__, PRNT_ERROR, "could not open file");
    }

    fread(buffer, 1, BUF_SIZE, ifp);
    if (create_file(name, buffer) != 0) {
        print_func(__func__, PRNT_ERROR, "could not create file");
    } else print_func(__func__, PRNT_SUCCESS, "file correctly created");
}*/
