#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
//#include <string.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <signal.h>
//#include <sys/wait.h>
#include "macros.h"
#include "startup_functions.h"

int main(int argc, char** argv){
        if(!(argc >= MIN_ARGS && correct_arguments(argv))) { //Minimum number of arguments: program name, root directory, -name <name> | -type <type> | -perm <perm>, -print | -delete (exec takes more args)
                print_usage();
                exit(-1);
        }
        DIR* root_dir;
        if((root_dir = opendir(argv[1])) == NULL) {
                printf("ERROR: Root search directory does not exist!\n");
                exit(-1);
        }
        printf("Good job\n");
        exit(0);
}
