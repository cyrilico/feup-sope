#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "macros.h"
#include "file_processing.h"

static int equals(const char* comparedTo, const char* toCompare){
        return strncmp(comparedTo, toCompare, strlen(toCompare)) == OK;
}

int notDotorDotDot(const char* str){
        return (!equals(str,".") && !equals(str, "..")) ? OK : ERROR;
}

/* Value from argv[3] that identifies entry/entries to be processed
   Static so it can't be accessed by other files
   Its value will be used by one of the three functions below (can be parsed later depending on which function it is)
 */
static char* identifying_value;

int identify_file_by_name(struct dirent* current, char* current_path){
        printf("IDENTIFYING BY NAME, RECEIVED %s\n", identifying_value);
        return equals(current->d_name, identifying_value) ? OK : ERROR;
}

int identify_file_by_type(struct dirent* current, char* current_path){
        //printf("IDENTIFYING BY TYPE, RECEIVED %s\n", identifying_value);

        struct stat temp;

        if(!IS_OK(lstat(current_path, &temp)))
                return ERROR;

        char type_chosen = identifying_value[0];
        mode_t file_type_mask = temp.st_mode & S_IFMT;

        /*
           Only checking for POSIX defined file types
         */
        switch(type_chosen) {
        case 'b':
                return file_type_mask & S_IFBLK ? OK : ERROR;
        case 'c':
                return file_type_mask & S_IFCHR ? OK : ERROR;
        case 'd':
                return file_type_mask & S_IFDIR ? OK : ERROR;
        case 'l':
                return file_type_mask & S_IFLNK ? OK : ERROR;
        case 'p':
                return file_type_mask & S_IFIFO ? OK : ERROR;
        case 'f':
                return file_type_mask & S_IFREG ? OK : ERROR;
        case 's':
                return file_type_mask & S_IFSOCK ? OK : ERROR;
        default:
                return ERROR;
        }
}

int identify_file_by_perm(struct dirent* current, char* current_path){
      //  printf("IDENTIFYING BY PERM, RECEIVED %s\n", identifying_value);
        unsigned permissions = strtol(identifying_value, NULL, 8);
      //  printf("PERMISSIONS REQUIRED AFTER CONVERSION: %o\n", permissions);

        struct stat temp;

        if(!IS_OK(lstat(current_path, &temp)))
                return ERROR;

        mode_t perm_mask = temp.st_mode & ~S_IFMT;
      //  printf("CURRENT ENTRY PERMISSIONS: %o\n", perm_mask);

        return perm_mask == permissions ? OK : ERROR;
}

void set_matching_function(fileMatchFunction* func, char* identifier_type, char* identifier_value){
        identifying_value = identifier_value;
        if(equals("-name", identifier_type))
                *func = &identify_file_by_name;
        else if(equals("-type", identifier_type))
                *func = &identify_file_by_type;
        else if(equals("-perm", identifier_type))
                *func = &identify_file_by_perm;
        else //Should never reach this case
                fprintf(stderr, "Somehow, none of the three valid identifiers have been passed?!\n");
}
