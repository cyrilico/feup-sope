#include <stdlib.h>
#include "s_macros.h"
#include "s_aux_functions.h"

static int capacity;

int read_capacity(char* arg){
  unsigned long capacity_desired = strtoul(arg, NULL, 10);

  if(capacity_desired == 0)
    return ERROR;

  capacity = capacity_desired;

  return OK;
}

int get_capacity(){
  return capacity;
}
