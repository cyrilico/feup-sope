#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "s_macros.h"
#include "s_aux_functions.h"

static sauna_info general_info;

#define NS_TO_MS(t) (t)/1e6

long get_ms_since_startup(){
        struct timespec current_time;
        if(clock_gettime(CLOCK_REALTIME, &current_time) == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                return ERROR;
        }
        return NS_TO_MS(current_time.tv_nsec - general_info.starting_time.tv_nsec);
}

int read_capacity(char* arg){
        unsigned long capacity_desired = strtoul(arg, NULL, 10);

        if(capacity_desired == 0)
                return ERROR;

        general_info.capacity = capacity_desired;

        //Take advantage of function to initialize time parameter
        if(clock_gettime(CLOCK_REALTIME, &general_info.starting_time) == ERROR)
                printf("Sauna: %s\n", strerror(errno));

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

void close_entry_fd(){
        printf("Sauna: Closing entry fd\n");
        close(general_info.requests_received_fd);
}

void close_rejected_fd(){
        printf("Sauna: Closing rejected fd\n");
        close(general_info.requests_rejected_fd);
}
