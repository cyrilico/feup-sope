#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
//#include <signal.h> //Don't delete, at least for now
#include "g_aux_functions.h"

int main(int argc, char** argv){
        srand(time(NULL));
        if(argc < 3)
                exit(ERROR);
        if(read_requests_info(argv) != OK) {
                printf("Error reading initial info\n");
                exit(ERROR);
        }

        int requests_sent_fd;
        while(((requests_sent_fd = open("/tmp/entrada", O_WRONLY | O_SYNC)) == ERROR))
                sleep(1);

        int requests_rejected_fd;
        if((requests_rejected_fd = open("/tmp/rejeitados", O_RDONLY)) == ERROR) {
                printf("ERROR: %s\n", strerror(errno));
                exit(ERROR);
        }

        printf("GERADOR: BOTH FIFOS OPEN\n");
        printf("GERADOR: FD'S. ENTRADA: %d REJEITADOS %d\n", requests_sent_fd, requests_rejected_fd);

        int i;
        for(i = 0; i < 5; i++) {
                request_info* temp = generate_request();
                if(write(requests_sent_fd, temp, sizeof(request_info)) == ERROR)
                        printf("ERROR: %s\n", strerror(errno));
                sleep(5);
        }

        close(requests_sent_fd);
        close(requests_rejected_fd);
        exit(OK);
}
