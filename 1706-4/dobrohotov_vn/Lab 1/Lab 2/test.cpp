//#include <iostream>
//#include "mpi.h"
//#include "windows.h"
//#include <queue>
//
//struct process
//{
//	int rank;
//	char s;
//};
//
//int main(int argc, char **argv)
//{
//	int rank;
//	int size;
//	std::queue <process> q;
//	int k;
//	int position;
//	int countOfReaders = 0;
//	bool free = false;
//	char *buff;
//	
//	MPI_Init(&argc, &argv);
//	MPI_Comm_size(MPI_COMM_WORLD, &size);
//	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//	MPI_Status status;
//	process p;
//	
//	if (size <3) {
//		std::cout << "Need minimum 3 process" << std::endl;
//		MPI_Finalize();
//		return -1;
//	}
//	if (rank == 0)
//	{
//		
//		while (1) 
//		{
//		
//		}
//	}
//	if (rank == 1) //readers
//	{
//		for (int i = 0; i < 2; i++) 
//		{
//		
//			position = 0;
//			p.rank = rank;
//			p.s = 'r';
//			MPI_Pack(&p.rank, 1, MPI_INT, buff, 5, &position, MPI_COMM_WORLD);
//			MPI_Pack(&p.rank, 1, MPI_CHAR, buff, 5, &position, MPI_COMM_WORLD);
//			MPI_Send(buff, position, MPI_PACKED, 0, 0, MPI_COMM_WORLD);
//		}
//	
//	}
//	if (rank == 2) //writers
//	{
//		for (int i = 0; i < 2; i++)
//		{
//			position = 0;
//			p.rank = rank;
//			p.s = 'w';
//			MPI_Pack(&p.rank, 1, MPI_INT, buff, 5, &position, MPI_COMM_WORLD);
//			MPI_Pack(&p.s, 1, MPI_CHAR, buff, 5, &position, MPI_COMM_WORLD);
//			MPI_Send(buff, position, MPI_PACKED, 0, 0, MPI_COMM_WORLD);
//		}
//	}
//
//	/*if (rank == 1)
//	{
//		tmp_rank = rank;
//		tmp_s = 'r';  
//		std::cout <<"first  "<< a << " " << c << std::endl;
//		position = 0;
//		k = 5;
//		buff = new char[k];
//		MPI_Pack(&a, 1, MPI_INT, buff, 5, &position, MPI_COMM_WORLD);
//		MPI_Pack(&c, 1, MPI_CHAR, buff, 5, &position, MPI_COMM_WORLD);
//		MPI_Send(buff, position, MPI_PACKED, 0, 0, MPI_COMM_WORLD);
//	}
//	if (rank == 0)
//	{
//		position = 0;
//		k = 5;
//		buff = new char[k];
//		MPI_Recv(buff, 5, MPI_PACKED, 1, 0, MPI_COMM_WORLD, &status);
//		MPI_Unpack(buff, 5, &position, &a,1,MPI_INT, MPI_COMM_WORLD);
//		MPI_Unpack(buff, 5, &position, &c, 1, MPI_CHAR, MPI_COMM_WORLD);
//		std::cout <<"Second   "<< a << " " << c <<"awdwa"<< std::endl;
//	}*
//	
//	MPI_Finalize();
//	return 0;
//}