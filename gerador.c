#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "g_aux_functions.h"

int main(int argc, char** argv){
        srand(time(NULL));
        if(argc < 3) {
                printf("Gerador: Incorrect number of arguments\n");
                exit(ERROR);
        }
        else if(read_requests_info(argv) != OK) {
                printf("Gerador: Incorrect initial information\n");
                exit(ERROR);
        }

        int requests_sent_fd, requests_rejected_fd;
        if(open_fifos(&requests_sent_fd, &requests_rejected_fd) == ERROR)
                exit(ERROR);

        //Testing writing to a .pid file (apparently, output like a text file)
        char file[50];
        sprintf(file, "/tmp/ger.%d", getpid());
        int statistics_fd;
        if((statistics_fd = open(file, O_WRONLY | O_CREAT | O_EXCL | O_SYNC)) == ERROR) {
                printf("Gerador: %s\n", strerror(errno));
                exit(ERROR);
        }

        printf("GERADOR: BOTH FIFOS OPEN. SENT: %d REJECTED: %d\n", requests_sent_fd, requests_rejected_fd);

        int i = get_number_of_requests();
        for(; i > 0; --i) {
                request_info* temp = generate_request();
                if(write(requests_sent_fd, temp, sizeof(request_info)) == ERROR)
                        printf("Gerador: %s\n", strerror(errno));
                char output[100];
                sprintf(output, "REQUEST MADE. SERIAL NUMBER %d, USAGE TIME %d, GENDER %c\n", temp->serial_number, temp->usage_time, temp->gender);
                if(write(statistics_fd, output, strlen(output)) == ERROR)
                        printf("Gerador: %s\n", strerror(errno));
                sleep(1);
        }

        close(requests_sent_fd);

        char msg[100];
        int status;
        while((status = read(requests_rejected_fd, msg, 100)), status > 0) {
                if(write(statistics_fd, msg, strlen(msg)) == ERROR)
                        printf("Gerador: %s\n", strerror(errno));
        }

        close(statistics_fd);
        close(requests_rejected_fd);
        exit(OK);
}
