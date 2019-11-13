#include <iostream>
#include "mpi.h"
#include "windows.h"

int main(int argc, char **argv)
{
	int rank;
	int size;
	bool stat;
	int countReaders = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	if (size < 1) {
		std::cout << "Error count process" << std::endl;
		return -2;
	}
	/*if (size <= 2)
	{
		std::cout << " need process > 2 " << std::endl;
		return -3;
	}*/
	//if (rank == 0) 
	//{
	//	int o;
	//	for (int i = 0; i <2; i++) {
	//		MPI_Recv(&o, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	//		std::cout << "elem " << o << std::endl;
	//		if (o == 1) {
	//			countReaders++;
	//			std::cout << "readers count " << countReaders << std::endl;
	//		}
	//		while (1) 
	//		{
	//			if (countReaders==0)
	//		
	//		}
	//			
	//	}
	//}

	//if (rank != 0 && rank % 2 == 0) //read		
	//{
	//	int b=0;
	//	for (int i = 0; i < 1; i++) {
	//		MPI_Send(&b, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
	//		Sleep(100);
	//	}
	//}
	//else //write
	//{
	//	int a = 1;
	//	for (int i = 0; i < 1; i++) {
	//		MPI_Send(&a, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
	//		Sleep(1000);
	//	}
	//}
	char b;
	char c;
	if (rank == 0)
	{
		MPI_Recv(&b, 1, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (!MPI_Recv(&c, 1, MPI_CHAR, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE))
			std::cout <<"Second send success "<< c<< std::endl;
		std::cout << "first send success " << b << std::endl;
	}

	if (rank == 1)
	{
		b = 'a';
		c = 'w';
		MPI_Send(&b, 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&c, 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	
}