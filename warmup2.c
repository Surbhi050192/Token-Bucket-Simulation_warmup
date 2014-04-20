#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "func.h"
#include "cs402.h"
#include "my402list.h"




int main(int argc,char *argv[])
{
    sigset(SIGINT,handler);
	//just to try the github
    //printf("run\n");
    //printf("argv num %d\n",argc);
    //int a=0;
    /********initial list***********/
    //initial list & bucket &mutex
    q1 = (My402List *)malloc(sizeof(My402List));
    q2 = (My402List *)malloc(sizeof(My402List));
    (void)My402ListInit(q1);
    (void)My402ListInit(q2);
    bucket=0;
    //FILE *fp = NULL;
    //initial
    data *para;
    para = (data*)malloc(sizeof(data));
    //default value of pkt
    (void)initdata(para);
    /*check input*/
    int check_i =1;
    int recor_file;
    while (check_i < argc)
    {
        //printf("%s\n",argv[check_i]);
        if(!strcmp(argv[check_i],"-lambda"))
        {
            //printf("right check\n");
            para->lambda = atof(argv[check_i+1]);
            if(para->lambda == 0)
            {
                fprintf(stderr,"the %s should be number\n", argv[check_i+1]);
                return 0;
            }
        }
        else if (!strcmp(argv[check_i],"-mu"))
        {
            //printf("wrong check\n");
            para->mu = atof(argv[check_i+1]);
            if(para->mu == 0)
            {
                fprintf(stderr,"the %s should be number\n",argv[check_i+1]);
                return 0;
            }
        }
        else if(!strcmp(argv[check_i],"-r"))
        {
            para->r = atof(argv[check_i+1]);
            if(para->r == 0)
            {
                fprintf(stderr,"the %s should be num\n", argv[check_i+1]);
                return 0;
            }
        }
        else if(!strcmp(argv[check_i],"-B"))
        {
            para->b = atoi(argv[check_i+1]);
            if(para->b == 0)
            {
                fprintf(stderr,"the %s should be num\n", argv[check_i+1]);
                return 0;
            }
        }
        else if(!strcmp(argv[check_i],"-P"))
        {
            para->p = atoi(argv[check_i+1]);
            if(para->p == 0)
            {
                fprintf(stderr,"the %s should be num\n", argv[check_i+1]);
                return 0;
            }
        }
        else if(!strcmp(argv[check_i],"-n"))
        {
            para->num = atoi(argv[check_i+1]);
            if(para->num == 0)
            {
                fprintf(stderr,"the %s should be num\n", argv[check_i+1]);
                return 0;
            }
        }
        else if(!strcmp(argv[check_i],"-t"))
        {
            para->fp = fopen(argv[check_i+1],"r");
            recor_file = check_i+1;
            if(para->fp == NULL)
            {
                fprintf(stderr,"cannot open the file\n");
                return 0;
            }
        }
        else
        {
            fprintf(stderr,"the %s cannot recognize\n", argv[check_i]);
            return 0;
        }
        check_i= check_i+2;
    }


    if(para->fp ==NULL)
    {
        /*print parameters*/
        printf("Emulation Parameters:\n");

        /*for no file*/
        printf("    number to arrive = %d\n    lambda = %f\n    mu = %f\n    r = %f\n    B = %d\n    p = %d\n",para->num,para->lambda,para->mu,para->r,para->b,para->p);
    }
    else
    {
        printf("Emulation Parameters:\n");

        /*for no file*/
        printf("    number to arrive is specified in file\n    r = %f\n    B = %d\n    tsfile = %s\n",para->r,para->b,argv[recor_file]);
    }
    /***********start*********************/
    sum_interarr_time=0;
    sum_serv_time=0;
    sum_sys_time=0;
    sum_q1_waittime = 0;
    sum_q2_waittime = 0;
    gettimeofday(&emu_start,NULL);
    printtime();
    printf("emulation begins\n");

    pthread_create(&arrive_id,
                   0,
                   arrival,
                   (void *)para);


    pthread_create(&token_id,
                   0,
                   tokenbucket,
                   (void *)para);


    pthread_create(&server_id,
                   0,
                   server,
                   (void *)para);

    pthread_join(arrive_id,0);
    //pthread_join(token_id,0);
    pthread_join(server_id,0);
    printtime();
    gettimeofday(&emu_end,NULL);
    printf("emulation ends\n");
    print_statistic();
    /***************end************************/
    return 0;

}
