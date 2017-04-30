typedef struct {
        int serial_number;
        int usage_time;
        char gender;
        int number_of_rejections;
}request_info;

int read_capacity(char* argv);
int get_capacity();
int create_fifos();
int open_fifos(int* requests_received_fd, int* requests_rejected_fd);
