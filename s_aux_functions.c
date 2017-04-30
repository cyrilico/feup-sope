#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "s_macros.h"
#include "s_aux_functions.h"

static int capacity;

int read_capacity(char* arg){
        unsigned long capacity_desired = strtoul(arg, NULL, 10);

        if(capacity_desired == 0)
                return ERROR;

        capacity = capacity_desired;

        return OK;
}

int get_capacity(){
        return capacity;
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

int open_fifos(int* requests_received_fd, int* requests_rejected_fd){
        if((*requests_received_fd = open("/tmp/entrada", O_RDONLY)) == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                return ERROR;
        }

        while(((*requests_rejected_fd = open("/tmp/rejeitados", O_WRONLY | O_SYNC)) == ERROR))
                sleep(1);

        return OK;
}
