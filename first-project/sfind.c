#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
//#include <string.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <signal.h>
//#include <sys/wait.h>
#include "startup_functions.h"
#include "file_processing.h"
#include "signal_functions.h"

int recursive_directory_search(DIR* root_directory){
  return 0;
}

int main(int argc, char** argv){
        /* Minimum number of arguments (must be in this order!!):
           program name, root directory to start search, -name <name> | -type <type> | -perm <perm>, -print | -delete (can be -exec but it takes more than the min args)
         */
        if(!(argc >= MIN_ARGS && IS_OK(correct_arguments(argv)))) {
                print_usage();
                exit(-1);
        }
        DIR* root_dir;
        if((root_dir = opendir(argv[1])) == NULL) {
                fprintf(stderr,"Root search directory does not exist!\n");
                exit(-1);
        }

        install_sigchld_handler();

        /* Testing if set_matching_function is working (it is) */
        fileMatchFunction f1;
        set_matching_function(&f1,argv[2],argv[3]);
        (*f1)(NULL);
        exit(0);

}
