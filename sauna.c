#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "s_aux_functions.h"

#define MS_TO_MICROS 1000

sem_t sauna_semaphore;

void* accept_request(void* request){
        sem_wait(&sauna_semaphore); //Will wait for opening if sauna is full
        //printf("Sauna: Request #%d entered sauna\n", ((request_info*)request)->serial_number);
        usleep(((request_info*)request)->usage_time * MS_TO_MICROS);
        sem_post(&sauna_semaphore); //Notify there is another free place
        //printf("Sauna: Request #%d exited sauna\n", ((request_info*)request)->serial_number);

        inc_number_of_served_requests(((request_info*)request));

        if(write_to_statistics(((request_info*)request), "SERVIDO") == ERROR)
                printf("Sauna(thread%ld): %s\n", pthread_self(), strerror(errno));

        return NULL;
}

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

        if (sem_init(&sauna_semaphore, 0, get_capacity()) != OK) {
                printf("Sauna: %s\n", strerror(errno));
                exit(ERROR);
        }

        if(create_fifos() == ERROR)
                exit(ERROR);

        //printf("SAUNA-PID%d: FIFOS CREATED\n", getpid());

        if(open_fifos() == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                exit(ERROR);
        }

        //printf("SAUNA-PID%d: BOTH FIFOS OPEN\n", getpid());

        if(open_statistics_file() == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                exit(ERROR);
        }

        if(receive_number_of_requests() == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                exit(ERROR);
        }

        request_info* current_request = (request_info*)(malloc(sizeof(request_info)));
        while(get_number_of_requests() > 0) {
                if(read_request(current_request) == ERROR)
                        printf("Sauna: %s\n", strerror(errno));

                inc_number_of_received_requests(current_request);
                dec_number_of_requests();

                if(write_to_statistics(current_request, "RECEBIDO") == ERROR)
                        printf("Sauna: %s\n", strerror(errno));

                int semaphore_value;
                sem_getvalue(&sauna_semaphore, &semaphore_value);
                int empty_sauna = (semaphore_value == get_capacity() ? 1 : 0);
                if(!empty_sauna && get_current_valid_gender() != current_request->gender) {
                        if(write_to_statistics(current_request, "REJEITADO") == ERROR)
                                printf("Sauna: %s\n", strerror(errno));
                        inc_number_of_rejected_requests(current_request);
                        if(++(current_request->number_of_rejections) < 3)
                          inc_number_of_requests();
                        if(send_rejected(current_request) == ERROR)
                                printf("Sauna: %s\n", strerror(errno));
                }
                else {
                        if(empty_sauna)
                                set_current_valid_gender(current_request->gender);
                        pthread_create(get_free_thread_id_pointer(), NULL, accept_request, (void*)current_request);
                }
                current_request = (request_info*)(malloc(sizeof(request_info)));
        }

        close_rejected_fd();

        wait_for_threads();

        close_entry_fd();

        print_final_statistics();

        close_statistics_fd();

        exit(OK);
}
