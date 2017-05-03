#include <time.h>

typedef struct {
        int capacity;

        int requests_received_fd;
        int requests_rejected_fd;
        int statistics_fd;

        struct timespec starting_time;
}sauna_info;

typedef struct {
        int serial_number;
        int usage_time;
        char gender;
        int number_of_rejections;
}request_info;

long get_ms_since_startup();

int read_capacity(char* argv);
int get_capacity();

int create_fifos();
int open_fifos();

int read_request(request_info* request);
int send_rejected(request_info* rejected);

void close_entry_fd(); //TODO: Add verification to return value from close()?
void close_rejected_fd(); //TODO: Add verification to return value from close()?
