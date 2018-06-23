#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <papi.h>
#include <omp.h>
#include <string.h>

#define NUM_THREADS 24 // maximum number of threads to run
#define SIZE 2048

// export OMP_NUM_THREADS=122
// export KMP_AFFINITY=compact


long long unsigned initial_time;
struct timeval begin;


void start (void) {
	gettimeofday(&begin, NULL);
}


void stop () {
	struct timeval end;
	gettimeofday(&end, NULL);
	long long duration = (end.tv_sec-begin.tv_sec)*1000000LL + end.tv_usec-begin.tv_usec;
	printf("%lld\n", duration);
}

/*__declspec (target (mic:0)) */ float a[SIZE * SIZE] __attribute__((align(64)));
/*__declspec (target (mic:0)) */ float b[SIZE * SIZE] __attribute__((align(64)));
/*__declspec (target (mic:0)) */ float c[SIZE * SIZE] __attribute__((align(64)));



int main (int argc, char *argv[]) {

	int i,j,k;

	for(i = 0; i < SIZE * SIZE; i++) {
		a[i] = ((float) rand()) / ((float) RAND_MAX);
		b[i] = 1;
		c[i] = 0;
	}

	start();

	#pragma offload(mic:0)
	#pragma omp parallel for private(j,k)
	for (i = 0; i < SIZE; ++i)
		for (j = 0; j < SIZE; ++j)
			for (k = 0; k < SIZE; ++k)
				c[i*SIZE+j] += a[i*SIZE+k] * b[k*SIZE+j];


	printf("time: ");
	stop();

	return 1;
}
