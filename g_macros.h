typedef struct{
  int max_usage_time;
  int number_of_requests;
}generator_info;

typedef struct{
  int serial_number;
  int usage_time;
  char gender;
}request_info;

#define OK 0
#define ERROR -1
