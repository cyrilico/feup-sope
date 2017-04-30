#include "g_macros.h"

typedef struct {
        //Read at startup
        int max_usage_time;
        int number_of_requests;
        //Updated throughout program execution
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
int open_fifos(int* requests_sent_fd, int* requests_rejected_fd);
