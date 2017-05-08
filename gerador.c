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
        while(1) {
                //If duplication fails, other thread has terminated which means sauna is ready to finish
                int temp_fd;
                if((temp_fd = dup(get_entry_fd())) == ERROR) {
                        printf("Gerador: Entry FD closed, exiting thread\n");
                        break;
                }
                else
                        close(temp_fd);

                pthread_mutex_lock(&queue_mutex);
                generate_request(); //Will do nothing if all requests have been generated (only processing rejected left)
                request_info* next_request = get_next_request();
                pthread_mutex_unlock(&queue_mutex);
                if(next_request == NULL)
                        continue;
                //printf("Gerador: Sending request %d\n", next_request->serial_number);
                if(send_request(next_request) == ERROR) {
                        printf("Gerador3: %s\n", strerror(errno));
                        exit(ERROR);
                }
                else{
                        if(write_to_statistics(next_request, "PEDIDO") == ERROR)
                                printf("Gerador4: %s\n", strerror(errno));
                }
        }

        close_rejected_fd();
        return NULL;
}

void* process_rejected_requests(void* nothing){
        request_info* current_rejected = (request_info*)(malloc(sizeof(request_info)));
        while(read_reject(current_rejected) == OK) {
                inc_number_of_rejected_requests(current_rejected); //TODO: CHECK: Being here we're counting a discarded rejection for the rejection count aswell. If we decide to not count it, just put this call inside below's 'if' brackets
                //printf("Gerador: Received back request %d\n", current_rejected->serial_number);
                if(current_rejected->number_of_rejections < 3) {
                        pthread_mutex_lock(&queue_mutex);
                        push_request(current_rejected);
                        pthread_mutex_unlock(&queue_mutex);
                        if(write_to_statistics(current_rejected, "REJEITADO") == ERROR)
                                printf("Gerador2: %s\n", strerror(errno));
                }
                else{
                        inc_number_of_discarded_requests(current_rejected);
                        if(write_to_statistics(current_rejected, "DESCARTADO") == ERROR)
                                printf("Gerador2: %s\n", strerror(errno));
                }

                current_rejected = (request_info*)(malloc(sizeof(request_info)));
        }

        close_entry_fd();
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

        if(send_number_of_requests() == ERROR) {
                printf("Gerador5: %s\n", strerror(errno));
                exit(ERROR);
        }

        pthread_t generator_thread, rejected_thread;
        pthread_create(&generator_thread, NULL, send_requests, NULL);
        pthread_create(&rejected_thread, NULL, process_rejected_requests, NULL);

        pthread_join(rejected_thread, NULL);
        pthread_join(generator_thread, NULL);

        close_statistics_fd();

        printf("Gerador: Final queue size: %d\n", get_queue_size());

        //TODO: Print final statistics
        close_statistics_fd();
        exit(OK);
}
