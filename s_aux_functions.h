#include <time.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct {
        int capacity;
        int number_of_requests; //TODO: Initialize

        int requests_received_fd;
        int requests_rejected_fd;
        int statistics_fd;

        double starting_time;

        char current_gender_in_sauna;
        pthread_t thread_ids[100]; //TODO: Use a macro here (max nr. of threads allowed per process?)
        int thread_id_index;
}sauna_info;

typedef struct {
        int serial_number;
        int usage_time;
        char gender;
        int number_of_rejections;
}request_info;

double get_ms_since_startup();

int get_number_of_requests();
void inc_number_of_requests();
void dec_number_of_requests();

int read_capacity(char* argv);
int get_capacity();

int create_fifos();
int open_fifos();

int receive_number_of_requests();

int open_statistics_file();
int write_to_statistics(request_info* request, const char* request_outcome);

char get_current_valid_gender();
void set_current_valid_gender(char gender);

pthread_t* get_free_thread_id_pointer();

int read_request(request_info* request);
int send_rejected(request_info* rejected);

void wait_for_threads();

void close_entry_fd(); //TODO: Add verification to return value from close()?
void close_statistics_fd(); //TODO: Add verification to return value from close()?
void close_rejected_fd(); //TODO: Add verification to return value from close()?
