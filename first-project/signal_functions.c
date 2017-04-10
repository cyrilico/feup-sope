#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "macros.h"
#include "signal_functions.h"

int termination_flag = 0;

void toggle_termination_flag(){
        termination_flag = (termination_flag + 1) % 2;
}

int flag_is_set(){
        return termination_flag == 1 ? OK : ERROR;
}

int confirm_termination(){
        printf("%d : Are you sure you want to terminate (Y/N)? ", getpid());
        char answer;
        scanf("%c", &answer);
        if(answer == 'Y' || answer == 'y')
                return OK;
        else{
                toggle_termination_flag();
                return ERROR;
        }
}

void sigint_handler(int signo){
        toggle_termination_flag();
        if(IS_OK(confirm_termination())) {
                char groupid[256];
                sprintf(groupid, "-%d", getpgrp());
                execlp("kill", "kill", groupid, NULL);
        }
}

int install_sigint_handler(){
        struct sigaction temp;
        temp.sa_handler = sigint_handler;
        temp.sa_flags = 0;
        sigfillset(&temp.sa_mask);
        return IS_OK(sigaction(SIGINT, &temp, NULL)) ? OK : ERROR;
}
