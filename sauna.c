#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "s_macros.h"

int main(int argc, char** argv){
        //TODO: Read initial program information from argv

        /* Create communication FIFOs */
        //TODO: Change to more fitting permissions
        if(mkfifo("/tmp/entrada", 0777) != OK) {
                if(errno != EEXIST) {
                        printf("ERROR: %s\n", strerror(errno));
                        exit(ERROR);
                }
        }

        //TODO: Change to more fitting permissions
        if(mkfifo("/tmp/rejeitados", 0777) != OK) {
                if(errno != EEXIST) {
                        printf("ERROR: %s\n", strerror(errno));
                        exit(ERROR);
                }
        }

        printf("SAUNA: FIFOS CREATED\n");

        int requests_received_fd;
        if((requests_received_fd = open("/tmp/entrada", O_RDONLY | O_NONBLOCK)) == ERROR){
          printf("ERROR: %s\n", strerror(errno));
          exit(ERROR);
        }

        int requests_rejected_fd;
        while(((requests_rejected_fd = open("/tmp/rejeitados", O_WRONLY | O_SYNC)) == ERROR))
          sleep(1);

        printf("SAUNA: BOTH FIFOS OPEN\n");


        exit(0);
}
