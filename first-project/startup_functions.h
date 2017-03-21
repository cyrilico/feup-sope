#pragma once

#include "macros.h"

/* Shows user the correct program call */
void print_usage();

/* Checks if arguments were given correctly (one of the three identifiers and one of the three operations)
 * Return: 0 if arguments are correct, -1 otherwise
 */
int correct_arguments(char** argv);
