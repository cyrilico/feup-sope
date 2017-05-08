#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "g_aux_functions.h"

#define S_TO_MS 1000
#define NS_TO_MS 1e-6

static generator_info general_info;
static requests_queue queue;
static int sequential_serial_number = 0;

double get_ms_since_startup(){
        struct timespec current_time_temp;
        if(clock_gettime(CLOCK_REALTIME, &current_time_temp) == ERROR) {
                printf("Gerador: %s\n", strerror(errno));
                return ERROR;
        }

        double current_time = (double)(current_time_temp.tv_sec*S_TO_MS + current_time_temp.tv_nsec*NS_TO_MS);
        //printf("Gerador: get_ms_since_startup: since epoch %f | difference %f\n", current_time, current_time - general_info.starting_time);

        return current_time - general_info.starting_time;
}

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

        //Take advantage of function to initialize time parameter
        struct timespec starting_time_temp;
        if(clock_gettime(CLOCK_REALTIME, &starting_time_temp) == ERROR)
                printf("Gerador: %s\n", strerror(errno));
        general_info.starting_time = (double)(starting_time_temp.tv_sec*S_TO_MS + starting_time_temp.tv_nsec*NS_TO_MS);

        //Actually do what the function is supposed to
        unsigned long max_requests = strtoul(argv[1], NULL, 10);
        unsigned long max_usage = strtoul(argv[2], NULL, 10);

        if(max_requests == 0 || max_usage == 0)
                return ERROR;

        general_info.number_of_requests = (int)max_requests;
        general_info.max_usage_time = (int)max_usage;

        //Also take advantage to initialize queue
        queue.first_index_free = 0;
        //Queue will never have more than the specified number of requests (can/will have less)
        queue.requests_queue = (request_info**)(malloc(general_info.number_of_requests*sizeof(request_info*)));

        return OK;
}

void generate_request(){
        if(general_info.number_of_requests == 0)
                return;

        request_info* result = (request_info*)(malloc(sizeof(request_info)));

        result->serial_number = sequential_serial_number++;
        result->usage_time = rand() % general_info.max_usage_time + 1;
        result->gender = (rand() % 2 == 0 ? 'F' : 'M');
        result->number_of_rejections = 0;

        general_info.number_of_requests--;

        inc_number_of_generated_requests(result);

        push_request(result);
}

int open_fifos(){
        while((general_info.requests_sent_fd = open("/tmp/entrada", O_WRONLY | O_SYNC)) == ERROR)
                sleep(1);

        if((general_info.requests_rejected_fd = open("/tmp/rejeitados", O_RDONLY)) == ERROR) {
                printf("Gerador: %s\n", strerror(errno));
                return ERROR;
        }

        return OK;
}

int open_statistics_file(){
        char file[50];
        sprintf(file, "/tmp/ger.%d", getpid());
        if((general_info.statistics_fd = open(file, O_WRONLY | O_CREAT | O_EXCL | O_SYNC, 0777)) == ERROR)
                return ERROR;
        return OK;
}

int send_number_of_requests(){
        if(write(general_info.requests_sent_fd, &general_info.number_of_requests, sizeof(int)) == ERROR)
                return ERROR;
        return OK;
}

int get_entry_fd(){
        return general_info.requests_sent_fd;
}

int send_request(request_info* request){
        if(write(general_info.requests_sent_fd, request, sizeof(request_info)) == ERROR)
                return ERROR;
        return OK;
}

int read_reject(request_info* rejected){
        int status;
        if(status = read(general_info.requests_rejected_fd, rejected, sizeof(request_info)), status == ERROR) {
                printf("Gerador: Fucked up while reading rejected request\n");
                return ERROR;
        }
        else if(status == 0) {
                printf("Gerador: End of rejected requests FIFO\n");
                return ERROR;
        }

        return OK;
}

int write_to_statistics(request_info* request, const char* request_outcome){
        char output[100];
        sprintf(output, "%.2f - %d - %d: %c - %d - %s\n", get_ms_since_startup(), getpid(), request->serial_number, request->gender, request->usage_time, request_outcome);
        if(write(general_info.statistics_fd, output, strlen(output)) == ERROR)
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

void inc_number_of_generated_requests(request_info* request){
        if(request->gender == 'M')
                general_info.number_of_generated_male_requests++;
        else
                general_info.number_of_generated_female_requests++;
}

void inc_number_of_rejected_requests(request_info* request){
        if(request->gender == 'M')
                general_info.number_of_rejected_male_requests++;
        else
                general_info.number_of_rejected_female_requests++;
}
void inc_number_of_discarded_requests(request_info* request){
        if(request->gender == 'M')
                general_info.number_of_discarded_male_requests++;
        else
                general_info.number_of_discarded_female_requests++;
}

void print_final_statistics(){
        printf("Gerador: %d requests were generated, %d male and %d female\n", general_info.number_of_generated_male_requests+general_info.number_of_generated_female_requests, general_info.number_of_generated_male_requests, general_info.number_of_generated_female_requests);
        printf("Gerador: %d requests were rejected by the sauna, %d male and %d female\n", general_info.number_of_rejected_male_requests+general_info.number_of_rejected_female_requests, general_info.number_of_rejected_male_requests, general_info.number_of_rejected_female_requests);
        printf("Gerador: %d requests were discarded, %d male and %d female\n", general_info.number_of_discarded_male_requests+general_info.number_of_discarded_female_requests, general_info.number_of_discarded_male_requests, general_info.number_of_discarded_female_requests);
}

request_info* get_next_request(){
        if(queue.first_index_free == 0)
                return NULL;

        request_info* result = queue.requests_queue[0];
        int i;
        for(i = 0; i < queue.first_index_free-1; i++)
                queue.requests_queue[i] = queue.requests_queue[i+1];

        queue.first_index_free--;

        return result;
}

void push_request(request_info* request){
        queue.requests_queue[queue.first_index_free] = request;
        queue.first_index_free++;
}

int get_queue_size(){
        return queue.first_index_free;
}
