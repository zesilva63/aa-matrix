#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <papi.h>
#include <omp.h>

#define NUM_THREADS 24 // maximum number of threads to run
#define TIME_RESOLUTION 1000000	// time measuring resolution (us)


long long unsigned initial_time;
struct timeval t;
double clearcache [30000000];


void clearCache (void) {
	for (int i = 0; i < 30000000; ++i)
		clearcache[i] = i;
}


void start (void) {
	gettimeofday(&t, NULL);
	initial_time = t.tv_sec * TIME_RESOLUTION + t.tv_usec;
}


long long unsigned stop () {
	gettimeofday(&t, NULL);
	long long unsigned final_time = t.tv_sec * TIME_RESOLUTION + t.tv_usec;

	return final_time - initial_time;
}


void prepareMatrices (float **a, float **b, float **c, int size) {
	int i, j;

	a = (float**) malloc(sizeof(float *) * size);
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

/*
*	c percorrido por linha fixo elemento
*   a percorrido por linha
*	b percorrido por coluna
*/
void multiplicationIJK (float** a, float** b, float** c, int size) {
	int i,j,k;
	for (i = 0; i < size; ++i) {
		for (j = 0; j < size; ++j) {
			for (k = 0; k < size; ++k) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}

void multiplicationIJKTranspose (float** a, float** b, float** c, int size) {
	int i,j,k;
	transpose(b,size);
	for (i = 0; i < size; ++i) {
		for (j = 0; j < size; ++j) {
			for (k = 0; k < size; ++k) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}

/*
*	c percorrido por linha
*   a percorrido por linha fixo elemento
*	b percorrido por linha
*/
void multiplicationIKJ (float** a, float** b, float** c, int size) {
	int i, k, j;
	for (i = 0; i < size; ++i) {
		for (k = 0; k < size; ++k) {
			for (j = 0; j < size; ++j) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}

/*
*	c percorrido por coluna
*   a percorrido por coluna
*	b percorrido por coluna fixo elemento
*/
void multiplicationJKI (float** a, float** b, float** c, int size) {
	int j, k, i;
	for (j = 0; j < size; ++j) {
		for (k = 0; k < size; ++k) {
			for (i = 0; i < size; ++i) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}

void multiplicationJKITranspose (float** a, float** b, float** c, int size) {
	int j, k, i;
	transpose(a,size);
	transpose(b,size);
	for (j = 0; j < size; ++j) {
		for (k = 0; k < size; ++k) {
			for (i = 0; i < size; ++i) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}


int main (int argc, char *argv[]) {
	
	int repetitions, size, i;

	float** a;
	float** b;
	float** c;

	// read arguments
	if (argc < 3) {
		printf("Usage: ./lab1 #size #repetitions\n");
		return 1;
	} else {
		size = atoi(argv[2]);
		repetitions = atoi(argv[3]);
	}
	
	prepareMatrices(a,b,c,size);

	// run the original code
	for (i = 0; i < repetitions; ++i) {
		clearCache();
		start();
		multiplicationIJK(a, b, c, size);
		printf("Time: %lld\n",stop());
	}

	return 1;
}
