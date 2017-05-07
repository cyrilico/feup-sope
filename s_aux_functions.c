#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "s_macros.h"
#include "s_aux_functions.h"

#define S_TO_MS 1e3
#define NS_TO_MS 1e-6

static sauna_info general_info;

double get_ms_since_startup(){
        struct timespec current_time_temp;
        if(clock_gettime(CLOCK_REALTIME, &current_time_temp) == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                return ERROR;
        }

        double current_time = (double)(current_time_temp.tv_sec*S_TO_MS + current_time_temp.tv_nsec*NS_TO_MS);
        //printf("Sauna: get_ms_since_startup: since epoch %f | difference %f\n", current_time, current_time - general_info.starting_time);

        return current_time - general_info.starting_time;
}

int read_capacity(char* arg){
        unsigned long capacity_desired = strtoul(arg, NULL, 10);

        if(capacity_desired == 0)
                return ERROR;

        general_info.capacity = capacity_desired;

        //Take advantage of function to initialize time parameter and pthread ids array
        struct timespec starting_time_temp;
        if(clock_gettime(CLOCK_REALTIME, &starting_time_temp) == ERROR)
                printf("Sauna: %s\n", strerror(errno));
        general_info.starting_time = (double)(starting_time_temp.tv_sec*S_TO_MS + starting_time_temp.tv_nsec*NS_TO_MS);
        printf("Sauna: ms since epoch %f\n", general_info.starting_time);

        general_info.thread_id_index = 0;

        return OK;
}

int get_capacity(){
        return general_info.capacity;
}

int create_fifos(){
        mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP; //Read and write for file owner and group owner

        if(mkfifo("/tmp/entrada", permissions) != OK) {
                if(errno != EEXIST) { //EEXIST would mean that couldn't make FIFO but only because it already exists
                        printf("Sauna: %s\n", strerror(errno));
                        return ERROR;
                }
        }

        if(mkfifo("/tmp/rejeitados", permissions) != OK) {
                if(errno != EEXIST) { //EEXIST would mean that couldn't make FIFO but only because it already exists
                        printf("Sauna: %s\n", strerror(errno));
                        return ERROR;
                }
        }

        return OK;
}

int open_fifos(){
        if((general_info.requests_received_fd = open("/tmp/entrada", O_RDONLY)) == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                return ERROR;
        }

        while((general_info.requests_rejected_fd = open("/tmp/rejeitados", O_WRONLY | O_SYNC)) == ERROR)
                sleep(1);

        return OK;
}

int open_statistics_file(){
        char file[50];
        sprintf(file, "/tmp/bal.%d", getpid());
        if((general_info.statistics_fd = open(file, O_WRONLY | O_CREAT | O_EXCL | O_SYNC)) == ERROR)
                return ERROR;
        return OK;
}

int write_to_statistics(request_info* request, const char* request_outcome){
        char output[100];
        sprintf(output, "%.2f - %d - %ld - %d: %c - %d - %s\n", get_ms_since_startup(), getpid(), pthread_self(), request->serial_number, request->gender, request->usage_time, request_outcome);
        if(write(general_info.statistics_fd, output, strlen(output)) == ERROR)
                return ERROR;
        return OK;
}

char get_current_valid_gender(){
        return general_info.current_gender_in_sauna;
}

void set_current_valid_gender(char gender){
        general_info.current_gender_in_sauna = gender;
}

pthread_t* get_free_thread_id_pointer(){
        if(general_info.thread_id_index == 100) //TODO: Use macro here (same one as in header file for array)
                return NULL;

        pthread_t* result = &general_info.thread_ids[general_info.thread_id_index];
        general_info.thread_id_index++;
        return result;
}


int read_request(request_info* request){
        if(read(general_info.requests_received_fd, request, sizeof(request_info)) > 0)
                return OK;
        return ERROR;
}

int send_rejected(request_info* rejected){
        if(write(general_info.requests_rejected_fd, rejected, sizeof(request_info)) == ERROR)
                return ERROR;
        return OK;
}

void wait_for_threads(){
        int i;
        for(i = 0; i < general_info.thread_id_index; i++)
                pthread_join(general_info.thread_ids[i], NULL);
}

void close_entry_fd(){
        printf("Sauna: Closing entry fd\n");
        close(general_info.requests_received_fd);
}

void close_statistics_fd(){
        close(general_info.statistics_fd);
}

void close_rejected_fd(){
        printf("Sauna: Closing rejected fd\n");
        close(general_info.requests_rejected_fd);
}
