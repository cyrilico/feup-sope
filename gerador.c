#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <assert.h>
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
                generate_request();
                request_info* next_request = get_next_request();
                if(send_request(next_request) == ERROR)
                        printf("Gerador: %s\n", strerror(errno));
                else{
                        if(write_to_statistics(next_request, "PEDIDO") == ERROR)
                                printf("Gerador: %s\n", strerror(errno));
                }
                sleep(1);
        }

        assert(get_queue_size() == 0);

        close_entry_fd();
        request_info stuff;
        while(read_reject(&stuff) == OK) {
                if(write_to_statistics(&stuff, "REJEITADO") == ERROR)
                        printf("Gerador: %s\n", strerror(errno));
        }

        close_statistics_fd();
        close_rejected_fd();
        exit(OK);
}
