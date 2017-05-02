#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "g_aux_functions.h"

static generator_info general_info;

static int sequential_serial_number = 0;

int get_number_of_requests(){
        return general_info.number_of_requests;
}

int read_requests_info(char** argv){
        //Take advantage of function call and initialize process information struct
        general_info.number_of_generated_male_requests = 0;
        general_info.number_of_generated_female_requests = 0;
        general_info.number_of_rejected_male_requests = 0;
        general_info.number_of_rejected_female_requests = 0;
        general_info.number_of_discarded_male_requests = 0;
        general_info.number_of_discarded_female_requests = 0;

        //Actually do what the function is supposed to
        unsigned long max_requests = strtoul(argv[1], NULL, 10);
        unsigned long max_usage = strtoul(argv[2], NULL, 10);

        if(max_requests == 0 || max_usage == 0)
                return ERROR;

        general_info.number_of_requests = (int)max_requests;
        general_info.max_usage_time = (int)max_usage;

        return OK;
}

request_info* generate_request(){
        request_info* result = (request_info*)(malloc(sizeof(request_info)));

        result->serial_number = sequential_serial_number++;
        result->usage_time = rand() % general_info.max_usage_time + 1;
        result->gender = (rand() % 2 == 0 ? 'F' : 'M');
        result->number_of_rejections = 0;

        return result;
}

int open_fifos(){
        while((general_info.requests_sent_fd = open("/tmp/entrada", O_WRONLY | O_SYNC)) == ERROR)
                sleep(1);

        if((general_info.requests_rejected_fd = open("/tmp/rejeitados", O_RDONLY)) == ERROR) {
                printf("Gerador: %s\n", strerror(errno));
                return ERROR;
        }

        printf("Gerador: Received | Rejected: %d | %d\n", general_info.requests_sent_fd, general_info.requests_rejected_fd);
        return OK;
}

int open_statistics_file(){
  char file[50];
  sprintf(file, "/tmp/ger.%d", getpid());
  if((general_info.statistics_fd = open(file, O_WRONLY | O_CREAT | O_EXCL | O_SYNC)) == ERROR)
    return ERROR;
  return OK;
}

int send_request(request_info* request){
  if(write(general_info.requests_sent_fd, request, sizeof(request_info)) == ERROR)
    return ERROR;
  return OK;
}

int read_reject(request_info* rejected){
  if(read(general_info.requests_rejected_fd, rejected, sizeof(request_info))  <= 0)
    return ERROR;
  return OK;
}

int write_to_statistics(char* str){
  if(write(general_info.statistics_fd, str, strlen(str)) == ERROR)
    return ERROR;
  return OK;
}

void close_entry_fd(){
  printf("Gerador: closing entry fd\n");
  close(general_info.requests_sent_fd);
}

void close_rejected_fd(){
  printf("Gerador: closing rejected fd\n");
  close(general_info.requests_rejected_fd);
}

void close_statistics_fd(){
  close(general_info.statistics_fd);
}
