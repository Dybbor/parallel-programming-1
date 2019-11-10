#include <iostream>
#include <fstream>
#include "mpi.h"
int main(int argc, char** argv) 
{
	int size; //количесво процессов
	int rank; //индентификатор процессаMPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Finalize();
	return 0;
}