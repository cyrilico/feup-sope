#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
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

        if(create_fifos() == ERROR)
                exit(ERROR);

        printf("SAUNA-PID%d: FIFOS CREATED\n", getpid());

        if(open_fifos() == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                exit(ERROR);
        }

        printf("SAUNA-PID%d: BOTH FIFOS OPEN\n", getpid());

        request_info* received[10];
        int index = 0;
        for(; index < 10; index++)
                received[index] = (request_info*)malloc(sizeof(request_info));

        index = 0;
        while(read_request(received[index]) == OK) {
                //int reject = rand() % 10;
                //if(reject < 7) {
                        printf("REQUEST RECEIVED. Serial nr.%d, expected usage time %d, gender %c\n", received[index]->serial_number, received[index]->usage_time, received[index]->gender);
                  //      received[index] = NULL;
                //}
                index++;
        }

        close_entry_fd();

        for(index = 0; index < 10; index++) {
                if(received[index] != NULL) {
                        printf("Sauna: Sending rejected back with serial number %d\n", received[index]->serial_number);
                        if(send_rejected(received[index]) == ERROR)
                                printf("Sauna: %s\n", strerror(errno));
                        sleep(1);
                }
        }

        close_rejected_fd();

        exit(OK);
}
