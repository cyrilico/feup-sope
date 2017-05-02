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
}generator_info;

typedef struct {
        int serial_number;
        int usage_time;
        char gender;
        int number_of_rejections; //To control when to possibly discard the request
}request_info;

int read_requests_info(char** argv);
int get_number_of_requests();

request_info* generate_request();

int open_fifos();
int open_statistics_file();

int send_request(request_info* request);
int read_reject(request_info* rejected);

int write_to_statistics(char* str); //TODO: Split into different functions to write made or rejected or discarded requests (each function receives a request as a parameter, string formed automatically inside)
void close_entry_fd(); //TODO: Add verification to close() return value?
void close_rejected_fd(); //TODO: Add verification to close() return value?
void close_statistics_fd(); //TODO: Add verification to close() return value?
