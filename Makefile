SHELL = /bin/sh

BIN_NAME = matrixMult


CC = gcc
CC2 = icpc

PAPI = -lpapi
LIB = -L/share/apps/papi/5.5.0/lib -I/share/apps/papi/5.5.0/include

OPT = -O2
OPT3 = -O3

OMP = -fopenmp
OMP2 = -openmp

FLAGS = -Wall -Wextra -Wno-unused-parameter 
REPORT = -vec-report3

ASSEMBLY = -S

compile: matrixMult.c
	$(CC) -o $(BIN_NAME) matrixMult.c $(LIB) $(PAPI) $(OPT) $(OMP)


assembly: matrixMult.c
	$(CC) $(ASSEMBLY) matrixMult.c $(LIB) $(PAPI) $(OPT) $(OMP) -o $(BIN_NAME) 


clean:
	rm -f $(BIN_NAME)


vector: matrixMult.c
	$(CC2) -o $(BIN_NAME) matrixMult.c $(ASSEMBLY) $(LIB) $(PAPI) $(OPT3) $(OMP2) $(REPORT)
