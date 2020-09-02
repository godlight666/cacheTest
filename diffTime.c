#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>


// get the user CPU time used
void gettime(double *cpu);
// syscall perf_event_open
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                       int cpu, int group_fd, unsigned long flags);



int main(int argc,char* argv[])
{
    if(argc != 2){
        printf("Usage: ./diffTime matrix_length");
        return 0;
    }
        
    int count = atoi(argv[1]);
    double cpu0,cpu1,cpu2,cpu3,cpu4,cpu5;

    //malloc the operation area in memory(heap)
    int *arr = (int*)malloc(sizeof(int) * count * count);

    struct perf_event_attr pe;
    long long misses;
    int fd;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx\n", pe.config);
        exit(EXIT_FAILURE);
    }

    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    printf("Measuring instruction count for this printf\n");

    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &misses, sizeof(long long));

    printf("Used %lld instructions\n", misses);

    close(fd);

    int i,j,r;

    //sequential read
    gettime(&cpu0);
    for(i=0;i<count;i++){
        for(j=0;j<count;j++){
            arr[i * count + j]=1;
            // printf("%d-%d ",i,j);
        }
    }
    gettime(&cpu1);

    //sequential write
    
    for(i=0;i<count;i++){
        for(j=0;j<count;j++){
            r = arr[i * count + j];
            // printf("%d-%d ",i,j);
        }
    }
    gettime(&cpu2);

    //random write
    for (j=0;j<count;j++){
        for (i=0;i<count;i++){
            arr[i*count+j] = 2;
        }
    }
    gettime(&cpu3);

    //random read
    for (j=0;j<count;j++){
        for (i=0;i<count;i++){
            r = arr[i*count+j];
        }
    }
    gettime(&cpu4);

    //free the memory area
    free(arr);

    //printf("按行遍历二维数组CPU时间差：%lf\n",cpu1-cpu0);
    //printf("按列遍历二维数组CPU时间差：%lf\n",cpu2-cpu1);
    printf("sequential write CPU time: %lf\n",cpu1-cpu0);
    printf("sequential read CPU time:  %lf\n",cpu2-cpu1);
    printf("random write CPU time:     %lf\n",cpu3-cpu2);
    printf("random read CPU time:      %lf\n",cpu4-cpu3);
    
    return 0;
}

// get the user CPU time used
void gettime(double *cpu)
{
    struct rusage ru;
    if(cpu != NULL)
    {
        getrusage(RUSAGE_SELF, &ru);
        *cpu = ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec * 1e-6;
    }
}

// syscall the perf_event_open
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                       int cpu, int group_fd, unsigned long flags){
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                          group_fd, flags);
    return ret;
}

