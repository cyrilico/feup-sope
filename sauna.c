#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "s_aux_functions.h"
#include "s_macros.h"

#define MS_TO_MICROS 1000

//TODO: HOW TO FIX THIS ISSUE?
//ISSUE: SEMAPHORE CAN'T BE IN STATIC STRUCT IN S_AUX_FUNCTIONS.C (THREADS SOMEHOW CAN'T SEE IT SO SEM_WAIT RESULTS IN INFINITE WAIT)
sem_t cenas;

void* accept_request(void* request){
        sem_wait(&cenas); //Will wait for opening if sauna is full
        usleep(((request_info*)request)->usage_time * MS_TO_MICROS);
        sem_post(&cenas);

        printf("Thread%ld: Request done, waiting to write to file\n", pthread_self());
        pthread_mutex_lock(get_mutex());
        printf("Thread%ld: Writing to file\n", pthread_self());
        if(write_to_statistics(((request_info*)request), "SERVIDO") == ERROR)
                printf("Sauna(thread%ld): %s\n", pthread_self(), strerror(errno));
        pthread_mutex_unlock(get_mutex());
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

        sem_init(&cenas, 0, get_capacity());

        if(create_fifos() == ERROR)
                exit(ERROR);

        printf("SAUNA-PID%d: FIFOS CREATED\n", getpid());

        if(open_fifos() == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                exit(ERROR);
        }

        printf("SAUNA-PID%d: BOTH FIFOS OPEN\n", getpid());

        if(open_statistics_file() == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                exit(ERROR);
        }

        //TEMPORARY
        request_info stuff;
        while(read_request(&stuff) == OK) {
                //Register that sauna got the request
                pthread_mutex_lock(get_mutex());
                if(write_to_statistics(&stuff, "RECEBIDO") == ERROR)
                        printf("Sauna: %s\n", strerror(errno));
                pthread_mutex_unlock(get_mutex());
                int reject = rand() % 10;
                //Register that sauna rejected the request
                if(reject < 5) {
                        pthread_mutex_lock(get_mutex());
                        if(write_to_statistics(&stuff, "REJEITADO") == ERROR)
                                printf("Sauna: %s\n", strerror(errno));
                        pthread_mutex_unlock(get_mutex());
                }
                //Accept the request, wait for its session to finish and register service
                else
                  pthread_create(get_free_thread_id_pointer(), NULL, accept_request, (void*)&stuff);
        }

        wait_for_threads();

        close_entry_fd();
        close_statistics_fd();
        close_rejected_fd();

        exit(OK);
}
