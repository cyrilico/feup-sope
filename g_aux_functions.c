#include <stdlib.h>
#include <stdio.h>
#include "g_aux_functions.h"

static generator_info general_info;

static int sequential_serial_number = 0;

int get_nr_requests(){
  return general_info.number_of_requests;
}

int read_requests_info(char** argv){
  unsigned long max_requests = strtoul(argv[1], NULL, 10);
  unsigned long max_usage = strtoul(argv[2], NULL, 10);

  if(max_requests == 0 || max_usage == 0)
    return ERROR;

  general_info.number_of_requests = (int)max_requests;
  general_info.max_usage_time = (int)max_usage;

  return OK;
}

request_info* generate_request(){
  request_info* result = (request_info*)(malloc(sizeof(request_info)));

  result->serial_number = sequential_serial_number++;
  result->usage_time = rand() % general_info.max_usage_time + 1;
  result->gender = (rand() % 2 == 0 ? 'F' : 'M');

  return result;
}
