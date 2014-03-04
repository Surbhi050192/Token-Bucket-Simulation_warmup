#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "func.h"
#include "cs402.h"
#include "my402list.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int total_num_pkt=1;
int pkt_label = 1;
int arrival_stop=0;
int total_num_token = 0;
int total_num_serv = 1;
int time_in_q1 = 0;
double in_q1_time = 0;
int time_in_q2 = 0;
double in_q2_time = 0;
int time_service = 0;
double service_time = 0;
int time_sys = 0;
double sys_time = 0;
long num_drop_token = 0;
long num_drop_pkt = 0;
int stop_flag = 1;
int stop_token =0;


void *arrival(void *para)
{
    sum_interarr_time=0;
    sum_serv_time=0;
    sum_sys_time=0;
    sum_q1_waittime=0;
    sum_q2_waittime=0;
    sqrsum_sys_time=0;
    data * param = (data*)para;
    /*for file read*/
    FILE *readline = param->fp;
    char *start_ptr;
    char *spa_ptr;
    char *end_ptr;
    char buf[1025];
    char inter[10];
    char needtoken[10];
    char needserv[10];
    /*record the last arrival time*/
    struct timeval last_arrival_time;
    last_arrival_time.tv_sec = emu_start.tv_sec;
    last_arrival_time.tv_usec = emu_start.tv_usec;
    int out_time=0;
    float out_intertime = 0;
    //printf("%f\n%f\n%f\n%d\n%d\n%d\n",param->lambda,param->mu,param->r,param->b,param->p,param->num);
    //int produce_num = param->num;
    //printf("%d\n", produce_num);
    //int i =0;
    useconds_t arrival_time;
    useconds_t timetosleep;
    useconds_t timeused = 0;
    struct timeval timestart;
    struct timeval timeend;
    //produce circle
    //while(i<produce_num)
    int line_num = 0;
    while(stop_flag)
    {
        pkt *newpkt;
        newpkt = (pkt*)malloc(sizeof(pkt));
        newpkt->pkt_num = pkt_label;
//        printf("%d\n",newpkt->pkt_num);
        /*read from file*/
        if(param->fp!=NULL)
        {
            //printf("open file successfully\n");
            //return 0;
            //if(line_num < param->num)
            fgets(buf,sizeof(buf),readline);
            start_ptr = buf;
            //printf("time back %d\n",line_num);
            //printf("%s\n",buf);

            if(line_num == 0)
            {
                /*read the num of pkts*/
                //end_ptr =strchr(start_ptr,'\n');
                end_ptr = start_ptr;
                while(*end_ptr!='\n')
                {
                    end_ptr++;
                }
                *end_ptr='\0';
                char a[5];
                strncpy(a,buf,4);
                param->num = atoi(a);
                line_num++;
                continue;
            }
            else if(line_num <= param->num)
            {
                /*get info*/
                //printf("in num: %d\n",line_num);
                //spa_ptr = strchr(start_ptr,' ');
                spa_ptr = start_ptr;
                while(*spa_ptr != ' ' && *spa_ptr != '\t')
                {
                    spa_ptr++;
                }
                *spa_ptr++='\0';
                strncpy(inter,start_ptr,9);
                start_ptr =spa_ptr;
                while(*start_ptr == ' ')
                {
                    start_ptr++;
                }
                //spa_ptr = strchr(start_ptr,' ');
                spa_ptr = start_ptr;
                while(*spa_ptr != ' ' && *spa_ptr != '\t')
                {
                    spa_ptr++;
                }
                *spa_ptr++='\0';
                strncpy(needtoken,start_ptr,9);
                start_ptr = spa_ptr;
                while(*start_ptr ==' ')
                {
                    start_ptr++;
                }
                //spa_ptr = strchr(start_ptr,'\n');
                spa_ptr = start_ptr;
                while(*spa_ptr != ' ' && *spa_ptr != '\t'&& *spa_ptr !='\n')
                {
                    spa_ptr++;
                }
                *spa_ptr = '\0';
                strncpy(needserv,start_ptr,9);

                line_num++;
                /**********************/
                arrival_time = atoi(inter)*1000;
                newpkt->trans_t = atoi(needserv)*1000;
                newpkt->token_num =atoi(needtoken);
                //printf("read service time:%d\n",newpkt->trans_t);
                //param->lambda = 1000/(double)lambda_inverse;
                //param->mu = 1000/(double)mu_inverse;

            }
            else
            {
                fclose(readline);
                break;
            }

        }
        /******************************/
        /*deterministic*/
        else
        {
            if(pkt_label>param->num)
            {
                break;
            }
//            printf("arrival time should be%f\n", (1/param->lambda)*1000000);
            arrival_time = (1/param->lambda)*1000000;
            /*create pkt object*/
            newpkt->token_num=param->p;
//      printf("%d\n",newpkt->token_num);
            newpkt->trans_t = (1/param->mu)*1000000;
        }
        /*****************************/
        if(arrival_time>10000000)
        {
            arrival_time = 10000000;
        }
        if(newpkt->trans_t > 10000000)
        {
            newpkt->trans_t = 10000000;
        }
        /*sleep: determine the sleep time*/
        timetosleep = arrival_time;
        if(timeused <=arrival_time)
        {
            timetosleep = arrival_time - timeused;
        }
        else
        {
            timetosleep = 0;
        }
//        printf("arrival time should be %d\n", timetosleep);
//        usleep(timetosleep);

/*
        //test the sleep time on nunki
        struct timeval mark1;
        struct timeval mark2;
        gettimeofday(&mark1,NULL);
        printf("\nthe sleep time should be: %dms\n", timetosleep);
        //splite
*/
        usleep(timetosleep);
        gettimeofday(&newpkt->ts1,NULL);
/*
        //splite resume
        gettimeofday(&mark2,NULL);
        int test_time = (mark2.tv_sec - mark1.tv_sec)*1000000+(mark2.tv_usec-mark1.tv_usec);
        //double test_intertime = (double)test_time/1000;
        printf("after sleep the actual interval is: %dms\n",test_time);
        //end the test
*/
        out_time=(newpkt->ts1.tv_sec-last_arrival_time.tv_sec)*1000000+(newpkt->ts1.tv_usec-last_arrival_time.tv_usec);
        last_arrival_time.tv_sec = newpkt->ts1.tv_sec;
        last_arrival_time.tv_usec = newpkt->ts1.tv_usec;
        out_intertime = (double)out_time/1000;
/*        out_time = arrival_time;
        out_intertime = (double)arrival_time/1000;
*/
        sum_interarr_time = sum_interarr_time + out_time;
        if(stop_flag == 0)break;
        printtime();
        printf("p%d arrives, needs %d tokens, inter-arrival time = %.3fms\n",newpkt->pkt_num,newpkt->token_num,out_intertime);
        if(newpkt->token_num > param->b)
        {
            /*in case of running into infinite loop
              should return false
            */
//            printf("require too many tokens\n");
            printtime();
            printf("p%d is dropped\n",pkt_label);
            //total_num_pkt++;////////
            pkt_label++;
            num_drop_pkt++;
            pthread_mutex_unlock(&mutex);
            continue;
        }
        /**********start*********/
        gettimeofday(&timestart,NULL);
        //printf("%d\n%d\n", newpkt->token_num,newpkt->trans_t);
        /*********lock**********/
//        printf("now in arrive\n");
        pthread_mutex_lock(&mutex);
        //wirte ts1
        /*p1 arrive print*/

        //printf("%d\n%d\n%d\n",newpkt->ts1.tv_sec,newpkt->ts1.tv_usec,bucket);
        /*enqueue to queue 1*/
        My402ListAppend(q1,newpkt);
        if(stop_flag == 0)break;
        printtime();
        printf("p%d enters Q1\n",newpkt->pkt_num);
        //count the num of pkt
        total_num_pkt++;
        pkt_label++;
        /*check token & move to queue2&broadcast*/
        My402ListElem *q1first = My402ListFirst(q1);
        pkt *changeq = (pkt *)q1first->obj;
        //printf("%d\n%d\n%d\n%d\n",changeq->ts1.tv_sec,changeq->ts1.tv_usec,changeq->token_num,changeq->trans_t);
        //check
        //bucket = 4; // for test
        int num_q1 = My402ListLength(q1);
        while(bucket >= changeq->token_num&&num_q1!=0)
        {
            bucket=bucket-changeq->token_num;
            gettimeofday(&changeq->ts2,NULL);
            My402ListUnlink(q1,q1first);
            //print leave q1
            time_in_q1 = (changeq->ts2.tv_sec - changeq->ts1.tv_sec)*1000000+changeq->ts2.tv_usec - changeq->ts1.tv_usec;
            in_q1_time = (double)time_in_q1/1000;
            sum_q1_waittime = sum_q1_waittime + time_in_q1;
            if(stop_flag == 0)break;
            printtime();
            printf("p%d leaves Q1, time in Q1 = %.3fms, token bucket now has %d token\n",changeq->pkt_num,in_q1_time,bucket);
            //move
            My402ListAppend(q2,changeq);
            if(stop_flag == 0)break;
            printtime();
            printf("p%d enters Q2\n", changeq->pkt_num);
            gettimeofday(&changeq->ts3,NULL);

            /*
                            My402ListElem *q2first = My402ListFirst(q2);
                            pkt *changeq2 = (pkt *)q2first->obj;
                            printf("%d\n%d\n%d\n%d\n",changeq2->ts1.tv_sec,changeq2->ts1.tv_usec,changeq2->token_num,changeq2->trans_t);
                            printf("%d\n",bucket);
                            int lengthq1 = My402ListLength(q1);
                            printf("%d\n",lengthq1);
            */
            //boradcast
            if(q2->num_members == 1)
            {
                pthread_cond_signal(&cv);
            }
            num_q1 = My402ListLength(q1);
            if(num_q1!=0)
            {
                    //printf("in\n");
                    q1first = My402ListFirst(q1);
                    changeq = ((pkt *)q1first->obj);
            }
        }
        //unlock
        pthread_mutex_unlock(&mutex);
        /*************end***********************/
        //actual time &sleep time
        gettimeofday(&timeend,NULL);
        timeused = (timeend.tv_sec-timestart.tv_sec)*1000000+(timeend.tv_usec-timestart.tv_usec);
        //printf("%d timeused\n",timeused);
        //printf("time to sleep %d\n",timetosleep);
        //continue circle
        //i=i+1;
    }
    arrival_stop=1;
    //printf("total num pkt: %d\n",total_num_pkt);
    return (0);
}


void *tokenbucket(void *para)
{
    data *param=(data*)para;
    useconds_t increm_time = (1/param->r)*1000000;
    useconds_t timetosleep = increm_time;
    useconds_t timeused = 0;
    struct timeval timestart;
    struct timeval timeend;
    while(stop_flag)
    {
        if(arrival_stop==1&&q1->num_members == 0)
        {
            break;
        }
        /*sleep*/
        if(timeused <=increm_time)
        {
            timetosleep = increm_time - timeused;
        }
        else
        {
            timetosleep = 0;
        }
        usleep(timetosleep);
        if(arrival_stop==1&&q1->num_members == 0)
        {
            break;
        }
        /***********start***************/
        gettimeofday(&timestart,NULL);
        /********lock***********/
        pthread_mutex_lock(&mutex);
//        printf("now in token\n");
        //printf("%d\n",param->b);
        //increment
        total_num_token++;
        if(stop_flag == 0)break;
        printtime();
        if(bucket < param->b)
        {
            bucket = bucket +1;
            printf("token t%d arrives, token bucket now has %d token(s)\n",total_num_token,bucket);
        }
        else
        {
            num_drop_token++;
            printf("token t%d arrives, dropped\n",total_num_token);
        }
        //printf("%d\n",bucket);
        //check queue1
        int num_q1 = My402ListLength(q1);
        if(num_q1!=0)
        {
            My402ListElem *q1first = My402ListFirst(q1);
            pkt *changeq = (pkt *)q1first->obj;
            //printf("%d\n%d\n%d\n%d\n",changeq->ts1.tv_sec,changeq->ts1.tv_usec,changeq->token_num,changeq->trans_t);
            //check & move
            //bucket = 4; // for test
            while(bucket >= changeq->token_num&&num_q1!=0)
            {
                bucket=bucket-changeq->token_num;
                gettimeofday(&changeq->ts2,NULL);
                //printf("%d\n%d\n",changeq->ts2.tv_sec,changeq->ts2.tv_usec);
                My402ListUnlink(q1,q1first);
                time_in_q1 = (changeq->ts2.tv_sec - changeq->ts1.tv_sec)*1000000+changeq->ts2.tv_usec - changeq->ts1.tv_usec;
                in_q1_time = (double)time_in_q1/1000;
                sum_q1_waittime = sum_q1_waittime + time_in_q1;
                if(stop_flag == 0)break;
                printtime();
                printf("p%d leaves Q1, time in Q1 = %.3fms, token bucket now has %d token\n",changeq->pkt_num,in_q1_time,bucket);
                //move
                My402ListAppend(q2,changeq);
                if(stop_flag == 0)break;
                printtime();
                printf("p%d enters Q2\n", changeq->pkt_num);
                gettimeofday(&changeq->ts3,NULL);
                //printf("%d\n%d\n",changeq->ts3.tv_sec,changeq->ts3.tv_usec);

                /*
                                                My402ListElem *q2first = My402ListFirst(q2);
                                                pkt *changeq2 = (pkt *)q2first->obj;
                                                printf("%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",changeq2->ts1.tv_sec,changeq2->ts1.tv_usec,changeq2->ts2.tv_sec,changeq2->ts2.tv_usec,changeq2->ts3.tv_sec,changeq2->ts3.tv_usec,changeq2->token_num,changeq2->trans_t);
                                                //printf("%d\n",bucket);
                                                //int lengthq1 = My402ListLength(q1);
                                                //printf("%d\n",lengthq1);
                */
                //boradcast
                if(q2->num_members == 1)
                {
                    pthread_cond_signal(&cv);
                }
                //next one in q1
                num_q1 = My402ListLength(q1);
                if(num_q1!=0)
                {
                    //printf("in\n");
                    q1first = My402ListFirst(q1);
                    changeq = ((pkt *)q1first->obj);
                }

            }
        }
        //unlock
        pthread_mutex_unlock(&mutex);
        /************end******************/
        //actual time
        gettimeofday(&timeend,NULL);
        timeused = (timeend.tv_sec-timestart.tv_sec)*1000000+(timeend.tv_usec-timestart.tv_usec);
        //printf("%d timeused\n",timeused);
        //printf("time to sleep %d\n",timetosleep);
        //continue circle
    }
    pthread_cond_broadcast(&cv);
    stop_token = 1;
    return 0;

}


void *server(void *para)
{
//    data*param= (data*)para;
//    int total_num = param->num;
//    int k=0;
    //usleep(16000000);
    while (stop_flag)
    {
        /******lock**********/
//       usleep(10000);
        pthread_mutex_lock(&mutex);
//        printf("in server\n");
        //empty? to cv

        while(q2->num_members == 0 && stop_token == 0)
        {
            pthread_cond_wait(&cv,
                              &mutex);
        }
        //printf("%d\n",q2->num_members);
        //not empty dequeue
//        if(q2->num_members != 0)//////////
//        {////////////
//        printf("succ wait the 1st one\n");
        if(q2->num_members != 0)
        {
            My402ListElem *deque = My402ListFirst(q2);
            pkt * q2deque = (pkt*)deque->obj;
            My402ListUnlink(q2,deque);
            gettimeofday(&q2deque->ts4,NULL);

            time_in_q2 = (q2deque->ts4.tv_sec-q2deque->ts3.tv_sec)*1000000+q2deque->ts4.tv_usec-q2deque->ts3.tv_usec;
            in_q2_time = (double)time_in_q2/1000;
            sum_q2_waittime = sum_q2_waittime +time_in_q2;
            if(stop_flag == 0)break;
            printtime();
            printf("p%d leaves Q2, time in Q2 = %.3fms, begin service at S\n", q2deque->pkt_num,in_q2_time);
            //unlock
            pthread_mutex_unlock(&mutex);
            //sleep to server
            useconds_t server_sleep = q2deque->trans_t;
            usleep(server_sleep);
            //time of leavey
            gettimeofday(&q2deque->ts5,NULL);
            time_service = (q2deque->ts5.tv_sec-q2deque->ts4.tv_sec)*1000000+q2deque->ts5.tv_usec-q2deque->ts4.tv_usec;
            sum_serv_time = sum_serv_time + time_service;
            service_time = (double)time_service/1000;
            time_sys = (q2deque->ts5.tv_sec-q2deque->ts1.tv_sec)*1000000+q2deque->ts5.tv_usec-q2deque->ts1.tv_usec;
            sum_sys_time = sum_sys_time + time_sys;
            sqrsum_sys_time = sqrsum_sys_time + (double)time_sys/1000000*(double)time_sys/1000000;
            sys_time = (double)time_sys/1000;
            if(stop_flag == 0)break;
            printtime();
            printf("p%d departs from S, service time = %.3fms, time in system = %.3fms\n",q2deque->pkt_num,service_time,sys_time);

//            sqr_sys_time = ((double)time_sys/1000000)*((double)time_sys/1000000);


//        k=k+1;
            total_num_serv++;
        }
//        }//////////
        if(total_num_serv==total_num_pkt&&arrival_stop==1)
        {
//            printf("quie!?!!!\n");
            break;
        }
//        if(q2->num_members == 0)///////
//        {///////
//            pthread_mutex_unlock(&mutex);/////
//        }//////
    }
    return 0;

}

/********other functions************/
int initdata(data* para)
{
    para->lambda = 0.5;//0.5
    para->mu = 0.35;//0.35
    para->r = 1.5;//1.5
    para->b = 10;//10
    para->p = 3;//3
    para->num = 20; //20
    para->fp = NULL;
    return 1;
}

void printtime()
{
    gettimeofday(&recor_out,NULL);
    int time_passed = (recor_out.tv_sec-emu_start.tv_sec)*1000000+(recor_out.tv_usec-emu_start.tv_usec);
//    float out_time = time_passed/1000;
//    printf("%.3fms: ",out_time);Var(X) = E(X2) - [E(X)]2

    int out_bit[11];
    int to_recor;
    int to_divide=1;
    int i = 0;
    for(i = 0; i<11; i++)
    {
        to_recor = time_passed/to_divide;
        out_bit[i]=to_recor%10;
        to_divide = to_divide*10;
    }
    printf("%d%d%d%d%d%d%d%d.%d%d%dms: ",out_bit[10],out_bit[9],out_bit[8],out_bit[7],out_bit[6],out_bit[5],out_bit[4],out_bit[3],out_bit[2],out_bit[1],out_bit[0]);

}

void print_statistic()
{
    if((total_num_pkt-1) == 0)
    {
        printf("\nCannot caculate precisely without any packt go through the system\n");
        double avg_inter_time = (double)sum_interarr_time/1000000/(pkt_label-1);///////
        double avg_ser_time = (double)sum_serv_time/1000000/(total_num_pkt-1);///////
        double avg_sys_time = (double)sum_sys_time/1000000/(total_num_pkt-1);///////
        double avg_sqr_sys_time = sqrsum_sys_time/(total_num_pkt-1);
        double var_sys_time = avg_sqr_sys_time - avg_sys_time*avg_sys_time;//////////////////////
        unsigned long total_emu_time = (emu_end.tv_sec-emu_start.tv_sec)*1000000+emu_end.tv_usec-emu_start.tv_usec;
//    printf("sum_q1_waittime %lu total_emu_time %lu\n",sum_q1_waittime,total_emu_time);
        double avg_q1_num = (double)sum_q1_waittime/(double)total_emu_time;
        double avg_q2_num = (double)sum_q2_waittime/(double)total_emu_time;
        double prob_drop_token = (double)num_drop_token/(double)total_num_token;
        double prob_drop_pkt = (double)num_drop_pkt/(double)(pkt_label-1);
//    printf("sum of service time: %lu\n", sum_serv_time);
//    printf("total emu time: %lu\n", total_emu_time);
        double avg_num_ser = (double)sum_serv_time/(double)total_emu_time;
        printf("\nStatistics:\n\n");
        printf("    average packet inter-arrival time = %.6gs\n",avg_inter_time);////
        printf("    average packet service time = %.6gs\n\n",avg_ser_time);///////
        printf("    average number of packets in Q1 = %.6g\n",avg_q1_num);
        printf("    average number of packets in Q2 = %.6g\n",avg_q2_num);
        printf("    average number of packets at S = %.6g\n\n", avg_num_ser);
        printf("    average time a packet spent in system = %.6gs\n",avg_sys_time);/////
        printf("    standard deviation for time spent in system = %.6g\n\n", sqrt(var_sys_time));
        printf("    token drop probability = %.6g\n", prob_drop_token);
        printf("    paket drop probability = %.6g\n", prob_drop_pkt);
    }
    else
    {
        double avg_inter_time = (double)sum_interarr_time/1000000/(pkt_label-1);///////
        double avg_ser_time = (double)sum_serv_time/1000000/(total_num_pkt-1);///////
//        printf("sum_sys_time%llu\n",sum_sys_time);
//        printf("total pkt: %d\n",total_num_pkt-1);
        double avg_sys_time = (double)sum_sys_time/1000000/(total_num_pkt-1);///////
        double avg_sqr_sys_time = sqrsum_sys_time/(total_num_pkt-1);
        double var_sys_time = avg_sqr_sys_time - avg_sys_time*avg_sys_time;//////////////////////
        unsigned long total_emu_time = (emu_end.tv_sec-emu_start.tv_sec)*1000000+emu_end.tv_usec-emu_start.tv_usec;
//    printf("sum_q1_waittime %lu total_emu_time %lu\n",sum_q1_waittime,total_emu_time);
        double avg_q1_num = (double)sum_q1_waittime/(double)total_emu_time;
        double avg_q2_num = (double)sum_q2_waittime/(double)total_emu_time;
        double prob_drop_token = (double)num_drop_token/(double)total_num_token;
        double prob_drop_pkt = (double)num_drop_pkt/(double)(pkt_label-1);
//    printf("sum of service time: %lu\n", sum_serv_time);
//    printf("total emu time: %lu\n", total_emu_time);
        double avg_num_ser = (double)sum_serv_time/(double)total_emu_time;
        printf("\nStatistics:\n\n");
        printf("    average packet inter-arrival time = %.6gs\n",avg_inter_time);////
        printf("    average packet service time = %.6gs\n\n",avg_ser_time);///////
        printf("    average number of packets in Q1 = %.6g\n",avg_q1_num);
        printf("    average number of packets in Q2 = %.6g\n",avg_q2_num);
        printf("    average number of packets at S = %.6g\n\n", avg_num_ser);
        printf("    average time a packet spent in system = %.6gs\n",avg_sys_time);/////
        printf("    standard deviation for time spent in system = %.6g\n\n", sqrt(var_sys_time));
        printf("    token drop probability = %.6g\n", prob_drop_token);
        printf("    paket drop probability = %.6g\n", prob_drop_pkt);
    }

}
/*************ctrl c****************/
void handler(int signo)
{
    printf("\n");
    stop_flag = 0;
    printf("wait for the server to deal with the last packet\n");
    //pthread_join(server_id,0);
    /*deque*/
    //q1
    //printf("pkt in q1:  %d\n",q1->num_members);
    My402ListElem *first_q1 = My402ListFirst(q1);
    if(first_q1 == NULL)
    {
        printtime();
        printf("there is no packet in q1 need to be removed\n");
    }
    while(first_q1!=NULL)
    {
        pkt*dequeue  = (pkt*)first_q1->obj;
        printtime();
        printf("p%d in q1 is removed\n", dequeue->pkt_num);
        My402ListUnlink(q1,first_q1);
        //free(first_q1);
        first_q1 = My402ListFirst(q1);
    }
    //q2
    //printf("pkt in q2: %d\n",q2->num_members);
    My402ListElem *first_q2 = My402ListFirst(q2);
    if(first_q2 == NULL)
    {
        printtime();
        printf("there is no packet in q2 need to be removed\n");
    }
    while(first_q2!=NULL)
    {
        printtime();
        pkt*dequeue2  = (pkt*)first_q2->obj;
        printf("p%d in q2 is removed\n", dequeue2->pkt_num);
        My402ListUnlink(q2,first_q2);
        //free(first_q1);
        first_q2 = My402ListFirst(q2);
    }
    printtime();
    printf("emulation stopped\n");
    //printf("total pkt %d\n",total_num_pkt);
    //printf("it is time to die\n");
    print_statistic();
    exit(1);

}
