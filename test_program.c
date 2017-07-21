#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/syscall.h>

#define BILLION 1000000000L
#define KB    1024

struct timespec T1, T2;

void withSync(char ch, int file_num, int record_size) {
	int idx = 0;
	int g = 0;
    int f[10000];
    char *buf = NULL;
    char filename[20];

	buf = (char*) malloc(1024 * 64);
	memset(buf, ch, 1024 * 64);

    for(idx = 0; idx < file_num; idx++) {
		sprintf(filename, "f_%d.txt", idx);

        f[idx] = open(filename, O_RDWR | O_CREAT);
        lseek (f[idx], 0, SEEK_END);

        write(f[idx], buf, (record_size-1) * KB);
        write(f[idx], buf, (1 * KB)-1);
        write(f[idx], "\n", 1); 

        if(idx == file_num-1) {
	        clock_gettime(CLOCK_MONOTONIC, &T1);
		    fsync(f[idx]);
            clock_gettime(CLOCK_MONOTONIC, &T2);
        }
        close (f[idx]);
    }
}

int main(int argc, const char * argv[])
{
	int file_num, record_size;
	unsigned long time;
    char ch, type;

	file_num = atoi(argv[1]);
	record_size = atoi(argv[2]);
    memcpy(&ch, argv[3], 1);
	
	withSync(ch, file_num, record_size);
	time = BILLION * (T2.tv_sec - T1.tv_sec) + T2.tv_nsec - T1.tv_nsec;
	printf("fsync() latency : %lu ns(%lu us, %lu ms)\n", time, time/1000, time/(1000*1000));

	return 0;
}

