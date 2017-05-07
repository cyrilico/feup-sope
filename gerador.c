#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "g_aux_functions.h"

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void* send_requests(void* nothing){
        while(get_number_of_requests_left() > 0) {
                pthread_mutex_lock(&queue_mutex);
                generate_request(); //Will do nothing if all requests have been generated (only processing rejected left)
                request_info* next_request = get_next_request();
                pthread_mutex_unlock(&queue_mutex);
                dec_number_of_requests_left();
                if(send_request(next_request) == ERROR){
                        printf("Gerador3: %s\n", strerror(errno));
                        exit(ERROR);
                }
                else{
                        if(write_to_statistics(next_request, "PEDIDO") == ERROR)
                                printf("Gerador4: %s\n", strerror(errno));
                }
        }

        return NULL;
}

void* process_rejected_requests(void* nothing){
        request_info* current_rejected = (request_info*)(malloc(sizeof(request_info)));
        while(read_reject(current_rejected) == OK) {
                pthread_mutex_lock(&queue_mutex);
                push_request(current_rejected);
                pthread_mutex_unlock(&queue_mutex);
                if(write_to_statistics(current_rejected, "REJEITADO") == ERROR)
                        printf("Gerador2: %s\n", strerror(errno));

                current_rejected = (request_info*)(malloc(sizeof(request_info)));
        }
        return NULL;
}

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

        printf("GERADOR-PID%d: BOTH FIFOS OPEN\n", getpid());

        if(open_statistics_file() == ERROR) {
                printf("Gerador1: %s\n", strerror(errno));
                exit(ERROR);
        }

        pthread_t generator_thread, rejected_thread;
        //pthread_create(&rejected_thread, NULL, process_rejected_requests, NULL);
        //close_rejected_fd();
        pthread_create(&generator_thread, NULL, send_requests, NULL);

        pthread_join(generator_thread, NULL);
        //pthread_join(rejected_thread, NULL);

        close_entry_fd();
        close_statistics_fd();

        printf("Gerador: Final queue size: %d\n", get_queue_size());
        close_rejected_fd();
        close_entry_fd();
        close_statistics_fd();
        exit(OK);
}
