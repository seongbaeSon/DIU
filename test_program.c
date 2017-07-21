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
#define NUM     4
#define SIZE    1024

int FILE_NUM;

struct timespec T1, T2;

void withSync(char ch) {
	int idx = 0;
	int g = 0;
    int f[10000];
    char *buf = NULL;
    char filename[20];

	buf = (char*) malloc(1024 * 64);
	memset(buf, ch, 1024 * 64);

    for(idx = 0; idx < FILE_NUM; idx++) {
		sprintf(filename, "f_%d.txt", idx);

        f[idx] = open(filename, O_RDWR | O_CREAT);
        lseek (f[idx], 0, SEEK_END);

        write(f[idx], buf, (NUM-1) * SIZE);
        write(f[idx], buf, (1 * SIZE)-1);
        write(f[idx], "\n", 1); 

        if(idx == FILE_NUM-1) {
	        clock_gettime(CLOCK_MONOTONIC, &T1);
		    fsync(f[idx]);
            clock_gettime(CLOCK_MONOTONIC, &T2);
        }
        close (f[idx]);
    }
}

int main(int argc, const char * argv[]) {
	int i = 0;
	unsigned long time;
    char ch;

    memcpy(&ch, argv[2], 1);
	FILE_NUM = atoi(argv[1]);
	
	withSync(ch);
	time = BILLION * (T2.tv_sec - T1.tv_sec) + T2.tv_nsec - T1.tv_nsec;
	printf("fsync() latency : %lu ns(%lu us, %lu ms)\n", time, time/1000, time/(1000*1000));

	return 0;
}

