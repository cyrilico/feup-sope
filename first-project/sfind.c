#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <fcntl.h>
#include <signal.h>
//#include <sys/wait.h>
#include "startup_functions.h"
#include "file_processing.h"
#include "signal_functions.h"
#include "macros.h"

static fileMatchFunction match_file;

int recursive_directory_search(DIR* root_directory, char* current_path_name){
        struct dirent* result;
        strcat(current_path_name, "/");
        while((result = readdir(root_directory)) != NULL) {
                struct stat result2;
                if(IS_OK(notDotorDotDot(result->d_name))) {
                        char* copyForLstat = (char*)malloc(256);
                        strcpy(copyForLstat, current_path_name);
                        if(IS_OK(lstat(strcat(copyForLstat,result->d_name), &result2))) {
                                /* Possibly remove this (for when using -type d as identifier),
                                   leaving only match_file function testing, but leave 'else if' (as an if) below
                                 */
                                if(S_ISREG(result2.st_mode)) {
                                        if(IS_OK((*match_file)(result)))
                                                printf("MATCHED: %s\n\n", copyForLstat);
                                }
                                else if(S_ISDIR(result2.st_mode)) {
                                        char* copyForRecursion = (char*)malloc(256);
                                        strcpy(copyForRecursion, copyForLstat);
                                        printf("Found sub-directory, starting all over in it\n\n");
                                        recursive_directory_search(opendir(copyForRecursion),copyForRecursion);
                                        free(copyForRecursion);
                                }
                        }
                        else
                                printf("ERROR IN LSTAT FOR %s\n\n", result->d_name);
                        free(copyForLstat);
                }
                else
                        printf("Comparison failed, so this is . or ..\n\n");
        }
        printf("Finished processing current directory\n\n");
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

        install_sigchld_handler(); //Every process will inherit this handler
        install_super_sigusr1_handler(); //Every process will override this handler

        /* Testing if set_matching_function is working */
        set_matching_function(&match_file,argv[2],argv[3]);

        char* stuff = (char*)malloc(1024);
        strcpy(stuff, argv[1]);
        if(IS_OK(recursive_directory_search(root_dir, stuff))) ;
        printf("Good job\n");
        exit(0);

}
