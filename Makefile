SHELL = /bin/sh

BIN_NAME = matrixMult

CC = gcc

PAPI = -lpapi

FLAGS = -O2 -L/share/apps/papi/5.5.0/lib -I/share/apps/papi/5.5.0/include -Wall -Wextra -fopenmp -Wno-unused-parameter
VEC = -O3 -L/share/apps/papi/5.5.0/lib -I/share/apps/papi/5.5.0/include -Wall -Wextra -fopenmp -Wno-unused-parameter -vec-report3

compile: matrixMult.c
	$(CC) -o $(BIN_NAME) matrixMult.c $(FLAGS) $(PAPI)


assembly: matrixMult.c
	$(CC) -S matrixMult.c -O2


clean:
	rm -f $(BIN_NAME)


vector: matrixMult.c
	$(CC) -o $(BIN_NAME) matrixMult.c $(VEC) $(PAPI)
