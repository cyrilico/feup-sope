#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "s_aux_functions.h"
#include "s_macros.h"

int main(int argc, char** argv){
        if(argc < 2){
          printf("Sauna: Incorrect number of arguments\n");
          exit(ERROR);
        }
        else if(read_capacity(argv[1]) == ERROR){
          printf("Sauna: Incorrect capacity specified\n");
          exit(ERROR);
        }

        /* Create communication FIFOs */
        //TODO: Change to more fitting permissions
        if(mkfifo("/tmp/entrada", 0777) != OK) {
                if(errno != EEXIST) { //EEXIST would mean that couldn't make FIFO but only because it already exists
                        printf("Sauna: %s\n", strerror(errno));
                        exit(ERROR);
                }
        }

        //TODO: Change to more fitting permissions
        if(mkfifo("/tmp/rejeitados", 0777) != OK) {
                if(errno != EEXIST) { //EEXIST would mean that couldn't make FIFO but only because it already exists
                        printf("Sauna: %s\n", strerror(errno));
                        exit(ERROR);
                }
        }

        printf("SAUNA: FIFOS CREATED\n");

        int requests_received_fd;
        if((requests_received_fd = open("/tmp/entrada", O_RDONLY)) == ERROR){
          printf("Sauna: %s\n", strerror(errno));
          exit(ERROR);
        }

        int requests_rejected_fd;
        while(((requests_rejected_fd = open("/tmp/rejeitados", O_WRONLY | O_SYNC)) == ERROR))
          sleep(1);

        printf("SAUNA: BOTH FIFOS OPEN\n");
        printf("SAUNA FD'S. ENTRADA: %d REJEITADOS: %d\n", requests_received_fd, requests_rejected_fd);

        request_info* received = (request_info*)(malloc(sizeof(request_info)));
        int status;
        while((status = read(requests_received_fd, received, sizeof(request_info))), status > 0)
          printf("REQUEST RECEIVED. Serial nr.%d, expected usage time %d, gender %c\n", received->serial_number, received->usage_time, received->gender);

        free(received);
        close(requests_received_fd);
        close(requests_rejected_fd);
        exit(0);
}
