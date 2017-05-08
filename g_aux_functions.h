#include <time.h>
#include <pthread.h>

#define OK 0
#define ERROR -1

typedef struct {
        //Read at startup
        int max_usage_time;
        int number_of_requests;

        //File descriptors for fifo and statistics file
        int requests_sent_fd;
        int requests_rejected_fd;
        int statistics_fd;

        //Updated throughout program execution
        int number_of_generated_male_requests;
        int number_of_generated_female_requests;

        int number_of_rejected_male_requests;
        int number_of_rejected_female_requests;

        int number_of_discarded_male_requests;
        int number_of_discarded_female_requests;

        double starting_time;
}generator_info;

typedef struct {
        int serial_number;
        int usage_time;
        char gender;
        int number_of_rejections; //To control when to possibly discard the request
}request_info;

double get_ms_since_startup();

int read_requests_info(char** argv);
int get_number_of_requests();

void generate_request();

int open_fifos();
int open_statistics_file();

int send_number_of_requests();

int get_entry_fd();

int send_request(request_info* request);
int read_reject(request_info* rejected);

int write_to_statistics(request_info* request, const char* request_outcome);

void close_entry_fd(); //TODO: Add verification to close() return value?
void close_rejected_fd(); //TODO: Add verification to close() return value?
void close_statistics_fd(); //TODO: Add verification to close() return value?

void inc_number_of_generated_requests(request_info* request);
void inc_number_of_rejected_requests(request_info* request);
void inc_number_of_discarded_requests(request_info* request);

void print_final_statistics();

typedef struct {
        int first_index_free;
        request_info** requests_queue;
}requests_queue;

request_info* get_next_request();
void push_request(request_info* request);
int get_queue_size(); //Necessary??
