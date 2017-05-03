#include <time.h>
#include "g_macros.h"

typedef struct {
        //Read at startup
        int max_usage_time;
        int number_of_requests;

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
}generator_info;

typedef struct {
        int serial_number;
        int usage_time;
        char gender;
        int number_of_rejections; //To control when to possibly discard the request
}request_info;

long get_ms_since_startup();

int read_requests_info(char** argv);
int get_number_of_requests();

void generate_request();

int open_fifos();
int open_statistics_file();

int send_request(request_info* request);
int read_reject(request_info* rejected);

//TODO: Split into different functions to write made or rejected or discarded requests.
//Each function receives a request and request state (PEDIDO, REJEITADO OU DESCARTADO) as parameters
int write_to_statistics(char* str);
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
