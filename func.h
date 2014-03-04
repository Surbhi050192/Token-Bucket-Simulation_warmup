#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "cs402.h"
#include "my402list.h"
pthread_t arrive_id,token_id,server_id;

typedef void (*sighandler_t)(int);
sighandler_t sigset(int sig, sighandler_t disp);

My402List *q1;
My402List *q2;
int bucket;
struct timeval emu_start;
struct timeval recor_out;
struct timeval emu_end;
unsigned long sum_interarr_time;
unsigned long sum_serv_time ;
unsigned long long sum_sys_time;
unsigned long long sum_q1_waittime;
unsigned long long sum_q2_waittime;
double sqr_sys_time;
double sqrsum_sys_time;

typedef struct{
    double lambda;
    double mu;
    double r;
    int b;
    int num;
    int p;
    FILE *fp;
}data;

typedef struct{
    int pkt_num;
    struct timeval ts1;
    struct timeval ts2;
    struct timeval ts3;
    struct timeval ts4;
    struct timeval ts5;
    int token_num;
    useconds_t trans_t;
}pkt;

void *arrival(void *para);
void *tokenbucket(void *para);
void *server(void *para);

int initdata(data* para);
void printtime();
void print_statistic();
void handler(int);
