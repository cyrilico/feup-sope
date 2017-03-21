#pragma once

#include <dirent.h>

typedef int (*fileMatchFunction)(struct dirent*);

int notDotorDotDot(const char* str);

/* Sets the comparison function to be used later during recursive directory search according to the type of file identifier inserted by the user */
void set_matching_function(fileMatchFunction* func, char* identifier_type, char* identifying_value);

int identify_file_by_name(struct dirent* current);
int identify_file_by_type(struct dirent* current);
int identify_file_by_perm(struct dirent* current);
