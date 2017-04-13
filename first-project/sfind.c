#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include "startup_functions.h"
#include "file_processing.h"
#include "signal_functions.h"
#include "macros.h"

static fileMatchFunction match_func;
static fileActionFunction action_func;

void cenas(int i){
  pause();
}

int recursive_directory_search(DIR* root_directory, char* current_path_name){
        struct dirent* result;
        if(current_path_name[strlen(current_path_name)-1] != '/')
                strcat(current_path_name, "/");
        while((result = readdir(root_directory)) != NULL && !IS_OK(flag_is_set())) {
                struct stat result2;
                if(IS_OK(notDotOrDotDot(result->d_name))) {
                        char* copyForLstat = (char*)malloc(256);
                        strcpy(copyForLstat, current_path_name);
                        if(IS_OK(lstat(strcat(copyForLstat,result->d_name), &result2))) {
                                if(IS_OK((*match_func)(result, copyForLstat)))
                                        if(!IS_OK((*action_func)(result, copyForLstat))) {
                                                printf("ERROR EXECUTING ACTION FOR %s\n", copyForLstat);
                                                continue;
                                        }
                                if(S_ISDIR(result2.st_mode)) {
                                        pid_t pid = fork();
                                        if(pid < 0) {
                                                printf("COULDN'T CREATE NEW PROCCESS, SKIPPING SUB-DIRECTORY\n");
                                                continue;
                                        }
                                        else if(pid == 0) {
                                                char* copyForRecursion = (char*)malloc(256);
                                                strcpy(copyForRecursion, copyForLstat);
                                                recursive_directory_search(opendir(copyForRecursion),copyForRecursion);
                                                free(copyForRecursion);
                                                exit(0);//return 0;
                                        }
                                        else{
                                                int status;
                                                waitpid(pid, &status, 0);
                                        }

                                }
                        }
                        else
                                printf("ERROR IN LSTAT FOR %s\n\n", result->d_name);
                        free(copyForLstat);
                }
        }
        if(IS_OK(flag_is_set())) {
                if(IS_OK(confirm_termination())){
                  char groupid[256];
                  sprintf(groupid, "-%d", getpgrp());
                  execlp("kill", "kill", "-s", "TERM", groupid, NULL);
                }
        }
        closedir(root_directory);
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

        /* Assigning entry matching function */
        set_matching_function(&match_func,argv[2],argv[3]);

        /* Assigning matched entries action function */
        set_action_function(&action_func, argv[4], (argv+5));

        /* Install signal handlers */
        if(!IS_OK(install_sigint_handler()) || !IS_OK(install_sigchld_handler()))
                exit(-1);

        char* full_path_holder = (char*)malloc(1024);
        strcpy(full_path_holder, argv[1]);
        if(!IS_OK(recursive_directory_search(root_dir, full_path_holder)))
              printf("Fodeu\n");
        exit(0);
}
