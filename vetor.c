#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define SIZE 32
#define BLOCKSIZE 16
#define NUMBLOCKS (SIZE/BLOCKSIZE)
#define TIME_RESOLUTION 1000000 // time measuring resolution (us)

struct timeval t;
double clearcache [30000000];
long long unsigned initial,final;
float a[SIZE][SIZE] __attribute__((aligned(16)));
float b[SIZE][SIZE] __attribute__((aligned(16)));
float res[SIZE][SIZE] __attribute__((aligned(16))) = {{0}};

int validateMatrix(void)
{
    int i, j, flag = 1;
    for (i = 0; i < SIZE && flag; i++){
        for (j = 0; j < SIZE && flag; j++){
            if (((res)[i][0]) != ((res)[i][j]))
          		flag = 0;
        }
    }
	return flag;
}

void clearCache (void) {
	unsigned i;
	for ( i = 0; i < 30000000; ++i)
		clearcache[i] = i;
}

void start(void)
{
 	gettimeofday(&t, NULL);
    initial = t.tv_sec * TIME_RESOLUTION + t.tv_usec;
}

long long unsigned stop(void)
{
	gettimeofday(&t, NULL);
	final = t.tv_sec * TIME_RESOLUTION + t.tv_usec;
	return final - initial;
}

void generate(void)
{
	int i,j;
	for(i=0;i<SIZE;i++)
	{
		for(j=0;j<SIZE;j++)
		{
			b[i][j] = 1;
			a[i][j] = (float) (rand()%10);
		}
	}

}

void blocking_matmult(void)
{
	int i,j,jj,kk;
    float aux[16]={0};
    for(jj=0; jj<SIZE; jj += BLOCKSIZE)
    {
        for(kk=0; kk<SIZE; kk += BLOCKSIZE)
        {
                for(i=0;i<SIZE;i++)
                {
                    for(j = jj; j< (jj + BLOCKSIZE); j++)
                    {
                            aux[0] = a[i][kk] * b[kk][j];
                            aux[1] = a[i][kk+1] * b[kk+1][j];
                            aux[2] = a[i][kk+2] * b[kk+2][j];
                            aux[3] = a[i][kk+3] * b[kk+3][j];
                            aux[4] = a[i][kk+4] * b[kk+4][j];
                            aux[5] = a[i][kk+5] * b[kk+5][j];
                            aux[6] = a[i][kk+6] * b[kk+6][j];
                            aux[7] = a[i][kk+7] * b[kk+7][j];
                            aux[8] = a[i][kk+8] * b[kk+8][j];
                            aux[9] = a[i][kk+9] * b[kk+9][j];
                            aux[10] = a[i][kk+10] * b[kk+10][j];
                            aux[11] = a[i][kk+11] * b[kk+11][j];
                            aux[12] = a[i][kk+12] * b[kk+12][j];
                            aux[13] = a[i][kk+13] * b[kk+13][j];
                            aux[14] = a[i][kk+14] * b[kk+14][j];
                            aux[15] = a[i][kk+15] * b[kk+15][j];

                            res[i][j] += aux[0] + aux[1] + aux[2] + aux[3] + aux[4]+ aux[5]+ aux[6]+ aux[7] + aux[8] + aux[9]+ aux[10]+ aux[11]+ aux[12]+ aux[13] + aux[14]+ aux[15];
                    }
                }
        }
    }
}

void sort_3(long long unsigned arr[3])
{
	long long unsigned swap;

	if (arr[0]>arr[1])
	{
		swap=arr[0];
		arr[0]=arr[1];
		arr[1]=swap;
	}
	if (arr[1] > arr[2])
	{
		swap=arr[1];
		arr[1]=arr[2];
		arr[2]=swap;
		if (arr[0] > arr[1])
		{
			swap=arr[0];
			arr[0]=arr[1];
			arr[1]=swap;
		}
	}
}

int run_and_time()
{
	int i,flag=0;
	long long unsigned best[3]={0},aux;

	for(i=0;i<8 && flag!=1;i++)
	{
		clearCache();

		start();

		blocking_matmult();

		aux = stop();

		if(!validateMatrix()){
			printf("Invalid Matrix!\n");
			return -1;
		}

		for(int j=0;j<SIZE;j++)
			for(int k=0;k<SIZE;k++)
				res[j][k]=0;

		if(i<3)
		{
			best[i] = aux;
		}
		if(i>=2)
		{
			sort_3(best);

			if((best[0] + best[0]*0.05) >= best[2] && (best[0] - best[0]*0.05) <= best[2])
			{
				flag=1;
			}
			else
			{
				best[2]=aux;
			}
		}
	}

	if(!flag)
		if(best[0]>=1000000)
			printf("K-Best time %llu* *10^6 us\n", best[0]/1000000);
		else if(best[0]>=1000)
			printf("K-Best time %llu* *10^3 us\n", best[0]/1000);
		else
			printf("K-Best time %llu* us\n", best[0]);
	else
	{
		long long unsigned avg=0;
		for(i=0;i<3;i++)
			avg+=best[i];

		if(avg/3>=1000000)
			printf("K-Best time %llu *10^6 us\n", avg/3000000);
		else if(avg/3>=1000)
			printf("K-Best time %llu *10^3 us\n", avg/3000);
		else
			printf("K-Best time %llu us\n", avg/3);
	}
	return 0;
}

int main(int argc, char*argv[])
{
	srand(time(NULL));

	generate();

	return (run_and_time());
}
