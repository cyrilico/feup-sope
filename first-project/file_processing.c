#include <stdio.h>
#include <string.h>
#include "macros.h"
#include "file_processing.h"

static int equals(const char* comparedTo, const char* toCompare){
        return strncmp(comparedTo, toCompare, strlen(toCompare)) == OK;
}

/* Value from argv[3] that identifies file(s) to be processed
   Static so it can't be accessed by other files
   Its value will be used by one of the three functions below (can be parsed later depending on which function it is)
 */
static char* identifying_value;

int identify_file_by_name(struct dirent* current){
  printf("IDENTIFYING BY NAME, RECEIVED %s\n", identifying_value);
  /* TODO: Implement function that checks if file whose information is in 'current' has the same name as stored in indentifying_value
    DON'T FORGET: in the struct, d_name holds the full path filename! Must filter to last characters (how to do that???)
    */
  return 0;
}

int identify_file_by_type(struct dirent* current){
  printf("IDENTIFYING BY TYPE, RECEIVED %s\n", identifying_value);
  /* TODO: Implement function that checks if file whose information is in 'current' has the same type as stored in indentifying_value
    DON'T FORGET: Must invoke stat/lstat over 'curren't as type information is stored in the return struct of that function
    REMEMBER: To check file types, use macros already defined (check documentation). To determine which macro to use, have a flag according to identifying_value?
    */
  return 0;
}

int identify_file_by_perm(struct dirent* current){
  printf("IDENTIFYING BY PERM, RECEIVED %s\n", identifying_value);
  /* TODO: Implement function that checks if file whose information is in 'current' has the same permissions as stored in indentifying_value
    DON'T FORGET: Must invoke stat/lstat over 'curren't as permissions information is stored in the return struct of that function
    REMEMBER: To check file permissions, use macros already defined (check documentation)
    */
  return 0;
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
