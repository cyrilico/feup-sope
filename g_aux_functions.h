#include <time.h>
#include <pthread.h>
#include "g_macros.h"

typedef struct {
        //Read at startup
        int max_usage_time;
        int number_of_requests_to_generate;
        int number_of_requests_left;

        //File descriptors for fifo and statistics file
        int requests_sent_fd;
        int requests_rejected_fd;
        int statistics_fd;

        //Updated throughout program execution (to be written to statistics file)
        int number_of_generated_male_requests;
        int number_of_generated_female_requests;

        int number_of_rejected_male_requests;
        int number_of_rejected_female_requests;

        int number_of_discarded_male_requests;
        int number_of_discarded_female_requests;

        struct timespec starting_time;

        pthread_mutex_t gerador_mutex;
}generator_info;

typedef struct {
        int serial_number;
        int usage_time;
        char gender;
        int number_of_rejections; //To control when to possibly discard the request
}request_info;

double get_ms_since_startup();

int read_requests_info(char** argv);
int get_number_of_requests_left();
void inc_number_of_requests_left();
void dec_number_of_requests_left();

void generate_request();

int open_fifos();
int open_statistics_file();

int create_mutex();
pthread_mutex_t* get_mutex();

int send_request(request_info* request);
int read_reject(request_info* rejected);

int write_to_statistics(request_info* request, const char* request_outcome);

void close_entry_fd(); //TODO: Add verification to close() return value?
void close_rejected_fd(); //TODO: Add verification to close() return value?
void close_statistics_fd(); //TODO: Add verification to close() return value?

typedef struct {
        int first_index_free;
        request_info** requests_queue;
}requests_queue;

request_info* get_next_request();
void push_request(request_info* request);
int get_queue_size(); //Necessary??
