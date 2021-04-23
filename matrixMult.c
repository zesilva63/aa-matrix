#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <papi.h>
#include <omp.h>

#define NUM_EVENTS 2
#define NUM_THREADS 24 // maximum number of threads to run
#define TIME_RESOLUTION 1000000 // time measuring milliseconds


long long unsigned initial_time;
struct timeval begin;
double clearcache [30000000];

float** a;
float** b;
float** c;


//int Events[NUM_EVENTS] = {PAPI_L2_DCR,PAPI_LD_INS};
//int Events[NUM_EVENTS] = {PAPI_L3_DCR,PAPI_L2_DCR};
//int Events[NUM_EVENTS] = {PAPI_L3_TCM,PAPI_L3_TCA};

int Events[NUM_EVENTS] = {PAPI_L3_TCM,PAPI_TOT_INS};


int EventSet = PAPI_NULL;
long long values[NUM_EVENTS];


void clearCache (void) {
	int j;
	int i;
	for (i = 0; i < 30000000; ++i)
		clearcache[i] = i;
}


void start (void) {
	gettimeofday(&begin, NULL);
}


void stop () {
	struct timeval end;
	gettimeofday(&end, NULL);
	long long duration = (end.tv_sec-begin.tv_sec)*1000000LL + end.tv_usec-begin.tv_usec;
	printf("%lld\n", duration);
}


void prepareMatrices (float **a, float **b, float **c, int size) {
	int i, j;


	b = (float**) malloc(sizeof(float *) * size);
	c = (float**) malloc(sizeof(float *) * size);

	for(i = 0 ; i < size ; i++) {
		a[i] = (float*) malloc(sizeof(float) * size);
		b[i] = (float*) malloc(sizeof(float) * size);
		c[i] = (float*) calloc(size, sizeof(float));
	}

	for (i = 0; i < size; ++i) {
		for (j = 0; j < size; ++j) {
			a[i][j] = ((float) rand()) / ((float) RAND_MAX);
			b[i][j] = 1;
		}
	}
}

void transpose (float **m, int size) {
	int i, j;
	float tmp;
	for(i = 0; i < size; ++i) {
		for(j = 0; j < i; ++j) {
			tmp = m[i][j];
			m[i][j] = m[j][i];
			m[j][i] = tmp;
		}
	}
}


void multiplicationIJK (float** a, float** b, float** c, int size) {
	int i,j,k;
	for (i = 0; i < size; ++i)
		for (j = 0; j < size; ++j)
			for (k = 0; k < size; ++k)
				c[i][j] += a[i][k] * b[k][j];
}

void multiplicationIJKTranspose (float** a, float** b, float** c, int size) {

	int i,j,k;
	transpose(b,size);
	for (i = 0; i < size; ++i)
		for (j = 0; j < size; ++j)
			for (k = 0; k < size; ++k)
				c[i][j] += a[i][k] * b[j][k];
}


void multiplicationIKJ (float** a, float** b, float** c, int size) {
	int i, k, j;
	for (i = 0; i < size; ++i)
		for (k = 0; k < size; ++k)
			for (j = 0; j < size; ++j)
				c[i][j] += a[i][k] * b[k][j];
}


void multiplicationJKI (float** a, float** b, float** c, int size) {
	int j, k, i;
	for (j = 0; j < size; ++j)
		for (k = 0; k < size; ++k)
			for (i = 0; i < size; ++i)
				c[i][j] += a[i][k] * b[k][j];
}


void multiplicationJKITranspose (float** a, float** b, float** c, int size) {
	int j, k, i;
	transpose(a,size);
	transpose(b,size);
	for (j = 0; j < size; ++j)
		for (k = 0; k < size; ++k)
			for (i = 0; i < size; ++i)
				c[j][i] += a[k][i] * b[j][k];

	transpose(c,size);
}



void multiplicationBlock(float **A, float **B, float **C, int size) {
	int jj, kk, i, j, k;
	int block_size = 16;
	float tmp;
	for (jj = 0; jj < size; jj += block_size)
	{
		for (kk = 0; kk < size; kk += block_size)
		{
			for (i = 0; i < size; i++)
			{
				for (j = jj; j < ((jj + block_size) > size ? size : (jj + block_size)); j++)
				{
					tmp = 0.0f;
					for (k = kk; k < ((kk + block_size) > size ? size : (kk + block_size)); k++)
					{
						tmp += A[i][k] * B[k][j];
					}
					C[i][j] += tmp;
				}
			}
		}
	}
}





void multiplicationBlockVec(float** __restrict__ A, float** __restrict__ B, float** __restrict__ C, int size) {
	int i = 0, j = 0, k = 0, jj = 0, kk = 0;
	float tmp;
	int block_size = 32;

		for (jj = 0; jj < size; jj += block_size)
		{
			for (kk = 0; kk < size; kk += block_size)
			{
				for (i = 0; i < size; i++)
				{
					for (j = jj; j < ((jj + block_size) > size ? size : (jj + block_size)); j++)
					{
						tmp = 0.0f;
						for (k = kk; k < ((kk + block_size) > size ? size : (kk + block_size)); k++)
						{
							tmp += A[i][k] * B[k][j];
						}
						C[i][j] += tmp;
					}
				}
			}
		}
}





void multiplicationBlockOMP(float** A, float** B, float** C, int size) {
	int i = 0, j = 0, k = 0, jj = 0, kk = 0;
	float tmp;
	int block_size = 16;
	#pragma omp parallel shared(A, B, C, size, block_size) private(i, j, k, jj, kk, tmp)
	{
		#pragma omp for schedule (static, 2)
		for (jj = 0; jj < size; jj += block_size)
		{
			for (kk = 0; kk < size; kk += block_size)
			{
				for (i = 0; i < size; i++)
				{
					for (j = jj; j < ((jj + block_size) > size ? size : (jj + block_size)); j++)
					{
						tmp = 0.0f;
						for (k = kk; k < ((kk + block_size) > size ? size : (kk + block_size)); k++)
						{
							tmp += A[i][k] * B[k][j];
						}
						C[i][j] += tmp;
					}
				}
			}
		}
	}
}





float** randomMatrix(int size) {
	int i, j;
	float** a = (float**) malloc(sizeof(float *) * size);

	for(i = 0; i < size; i++)
		a[i] = (float*) malloc(size * sizeof(float));

	//srand(time(NULL));
	for (i = 0; i < size; ++i)
		for (j = 0; j < size; j++)
			a[i][j] = ((float)rand() / (float)(RAND_MAX)) * 100 ;

	return a;
}

float** unitaryMatrix(int size) {
	int i, j;
	float** b = (float**) malloc(sizeof(float *) * size);

	for(i = 0; i < size; i++)
		b[i] = (float*) malloc(size * sizeof(float));

	for (i = 0; i < size; ++i)
		for (j = 0; j < size; j++)
			b[i][j] = 1;

	return b;
}

float** emptyMatrix(int size) {
	int i;
	float** c = (float**) malloc(sizeof(float *) * size);

	for(i = 0; i < size; i++)
		c[i] = (float*) calloc(size, sizeof(float));

	return c;
}

int main (int argc, char *argv[]) {

	int repetitions, size, i;

	size = 2048;
	repetitions = 5;
	a = randomMatrix(size);
	b = unitaryMatrix(size);
	c = emptyMatrix(size);

	PAPI_library_init(PAPI_VER_CURRENT);
	PAPI_create_eventset(&EventSet);
	PAPI_add_events(EventSet,Events,NUM_EVENTS);

	for (i = 0; i < repetitions; ++i) {
		clearCache();
		start();
		PAPI_start(EventSet);

		multiplicationIJKTranspose(a, b, c, size);
		multiplicationIJK(a, b, c, size);

		multiplicationBlockVec(a,b,c,size);

		PAPI_stop(EventSet,values);
		printf("time: ");
		stop();

		printf("RAM ACCESSES: %lld\n",values[0]);
		printf("INSTR: %lld\n",values[1]);

		double r = (double) values[0]/values[1];
		printf("RAM / INS: %f\n",r);


	}

	return 1;
}
