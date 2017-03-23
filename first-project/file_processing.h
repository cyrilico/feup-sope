#pragma once

#include <dirent.h>

typedef int (*fileMatchFunction)(struct dirent*, char*);
typedef int (*fileActionFunction)(struct dirent*, char*);

int notDotOrDotDot(const char* str);

/* Sets the comparison function to be used later during recursive directory search according to the type of file identifier inserted by the user */
void set_matching_function(fileMatchFunction* func, char* identifier_type, char* identifying_value);

int identify_file_by_name(struct dirent* current, char* current_path);
int identify_file_by_type(struct dirent* current, char* current_path);
int identify_file_by_perm(struct dirent* current, char* current_path);

/* Sets the action function to be applied later to every matched entry in the recursive directory search */
void set_action_function(fileActionFunction* func, char* action_name, char** command_args);

int print_file(struct dirent* current, char* current_path);
int delete_file(struct dirent* current, char* current_path);
int exec_command(struct dirent* current, char* current_path);
