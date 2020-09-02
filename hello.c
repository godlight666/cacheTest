#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<time.h>

void gettime(double *cpu)
{
    struct rusage ru;
    if(cpu != NULL)
    {
        getrusage(RUSAGE_SELF, &ru);
        *cpu = ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec * 1e-6;
    }
}

int main(){
    //double t1,t2;
    //gettime(&t1);
    clock_t t1 = clock();
    printf("hello world\n");
    printf("you are welcome\n");
    sleep(2);
    //gettime(&t2);
    clock_t t2 = clock();
    printf("the time is : %lf\n",(double)(t2-t1)/CLOCKS_PER_SEC);
}