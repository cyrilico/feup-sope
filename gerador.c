#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "g_aux_functions.h"

int main(int argc, char** argv){
  /*if(argc < 3)
    exit(ERROR);
  if(read_requests_info(argv) != OK){
    printf("Error reading initial info\n");
    exit(ERROR);
  }*/

  int requests_sent_fd;
  while(((requests_sent_fd = open("/tmp/entrada", O_WRONLY | O_SYNC)) == ERROR))
    sleep(1);

  int requests_rejected_fd;
  if((requests_rejected_fd = open("/tmp/rejeitados", O_RDONLY | O_NONBLOCK)) == ERROR){
    printf("ERROR: %s\n", strerror(errno));
    exit(ERROR);
  }

  printf("GERADOR: BOTH FIFOS OPEN\n");

  close(requests_sent_fd);
  close(requests_rejected_fd);
  exit(OK);
}
