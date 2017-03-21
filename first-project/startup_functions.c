#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include "macros.h"
#include "startup_functions.h"

void print_usage(){
        printf("USAGE: ./sfind (-name <filename> | -type <filetype> | -perm <file permissions>) (-print | -delete | -exec <command> [{}] ;)\n");
}

static int equals(const char* comparedTo, const char* toCompare){
        return strncmp(comparedTo, toCompare, strlen(toCompare)) == OK;
}

int correct_arguments(char** argv){
        if(!(equals("-name",argv[2]) || equals("-type",argv[2]) || equals("-perm",argv[2])))
                return ERROR;
        else if(!(equals("-print",argv[4]) || equals("-delete",argv[4]) || equals("-exec",argv[4])))
                return ERROR;
        return OK;
}
