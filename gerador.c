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

        if(open_fifos() == ERROR)
                exit(ERROR);

        //Testing writing to a .pid file (apparently, output like a text file)
        if(open_statistics_file() == ERROR) {
                printf("Gerador: %s\n", strerror(errno));
                exit(ERROR);
        }

        printf("GERADOR-PID%d: BOTH FIFOS OPEN\n", getpid());

        int i = get_number_of_requests();
        for(; i > 0; --i) {
                request_info* temp = generate_request();
                if(send_request(temp) == ERROR)
                        printf("Gerador: %s\n", strerror(errno));
                char output[100];
                sprintf(output, "REQUEST MADE. SERIAL NUMBER %d, USAGE TIME %d, GENDER %c\n", temp->serial_number, temp->usage_time, temp->gender);
                if(write_to_statistics(output) == ERROR)
                        printf("Gerador: %s\n", strerror(errno));
                sleep(1);
        }

        close_entry_fd();
        request_info stuff;
        while(read_reject(&stuff) == OK){
          char msg[100];
          sprintf(msg, "REQUEST REJECTED. SERIAL NUMBER %d, USAGE TIME %d, GENDER %c\n", stuff.serial_number, stuff.usage_time, stuff.gender);
          if(write_to_statistics(msg) == ERROR)
                  printf("Gerador: %s\n", strerror(errno));
        }

        close_statistics_fd();
        close_rejected_fd();
        exit(OK);
}
