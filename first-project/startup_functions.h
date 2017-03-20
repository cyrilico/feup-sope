#include <dirent.h>
#include "macros.h"

/* Shows user the correct program call */
void print_usage();

/* Checks if arguments were given correctly (one of the three identifiers and one of the three operations)
 * Return: 0 if arguments are correct, -1 otherwise
 */
int correct_arguments(char** argv);

typedef int (*fileMatchFunction)(struct dirent*);

/* Sets the comparison function to be used later during recursive directory search according to the type of file identifier inserted by the user */
void set_matching_function(fileMatchFunction* func, char* identifier_type, char* identifying_value);
