#include <time.h>
#include <semaphore.h>
#include <pthread.h>

#define OK 0
#define ERROR -1
#define MAX_THREADS 50000 //Hoping it's enough

typedef struct {
        int capacity;
        int number_of_requests;

        int requests_received_fd;
        int requests_rejected_fd;
        int statistics_fd;

        double starting_time;

        int number_of_received_male_requests;
        int number_of_received_female_requests;
        int number_of_rejected_male_requests;
        int number_of_rejected_female_requests;
        int number_of_served_male_requests;
        int number_of_served_female_requests;

        char current_gender_in_sauna;
        pthread_t thread_ids[MAX_THREADS];
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

void close_entry_fd();
void close_statistics_fd();
void close_rejected_fd();

void inc_number_of_received_requests(request_info* request);
void inc_number_of_rejected_requests(request_info* request);
void inc_number_of_served_requests(request_info* request);

void print_final_statistics();
