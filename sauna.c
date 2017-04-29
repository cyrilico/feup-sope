#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "s_aux_functions.h"
#include "s_macros.h"

int main(int argc, char** argv){
        srand(time(NULL));
        if(argc < 2) {
                printf("Sauna: Incorrect number of arguments\n");
                exit(ERROR);
        }
        else if(read_capacity(argv[1]) == ERROR) {
                printf("Sauna: Incorrect capacity specified\n");
                exit(ERROR);
        }

        /* Create communication FIFOs */
        mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP; //Read and write for file owner and group owner

        if(mkfifo("/tmp/entrada", permissions) != OK) {
                if(errno != EEXIST) { //EEXIST would mean that couldn't make FIFO but only because it already exists
                        printf("Sauna: %s\n", strerror(errno));
                        exit(ERROR);
                }
        }

        if(mkfifo("/tmp/rejeitados", permissions) != OK) {
                if(errno != EEXIST) { //EEXIST would mean that couldn't make FIFO but only because it already exists
                        printf("Sauna: %s\n", strerror(errno));
                        exit(ERROR);
                }
        }

        printf("SAUNA: FIFOS CREATED\n");

        int requests_received_fd;
        if((requests_received_fd = open("/tmp/entrada", O_RDONLY)) == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                exit(ERROR);
        }

        int requests_rejected_fd;
        while(((requests_rejected_fd = open("/tmp/rejeitados", O_WRONLY | O_SYNC)) == ERROR))
                sleep(1);

        printf("SAUNA: BOTH FIFOS OPEN\n");
        printf("SAUNA FD'S. ENTRADA: %d REJEITADOS: %d\n", requests_received_fd, requests_rejected_fd);

        request_info* received[10];
        int index = 0;
        for(; index < 10; index++)
                received[index] = (request_info*)malloc(sizeof(request_info));

        int status;
        index = 0;
        while((status = read(requests_received_fd, received[index], sizeof(request_info))) > 0) {
                int reject = rand() % 10;
                if(reject < 7) {
                        printf("REQUEST RECEIVED. Serial nr.%d, expected usage time %d, gender %c\n", received[index]->serial_number, received[index]->usage_time, received[index]->gender);
                        received[index] = NULL;
                }
                index++;
        }

        close(requests_received_fd);

        for(index = 9; index >= 0; index--) {
                if(received[index] != NULL) {
                        char msg[100];
                        sprintf(msg, "REQUEST %d REJECTED\n", received[index]->serial_number);
                        if(write(requests_rejected_fd, msg, strlen(msg)) == ERROR)
                                printf("Sauna: %s\n", strerror(errno));
                        sleep(1);
                }
        }

        close(requests_rejected_fd);

        exit(0);
}
