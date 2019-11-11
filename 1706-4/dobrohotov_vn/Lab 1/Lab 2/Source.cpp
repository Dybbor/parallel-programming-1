#include <iostream>
#include <fstream>
#include <string>
#include "Mutex.h"
#include "mpi.h"
#include "windows.h"

static int readCount = 0;
static Mutex *accessProcess;
 
int FindFreeProcess(Mutex *m,int size) 
{
	for (int i = 0; i < size; i++)
		if (m[i].Info())
			return i + 1;
	return 0;
}
void Print(int rank, bool m, int value, char ex) 
{
	std::cout << "rank " << rank << "\n mutex " << m << "\n value " << value << "\n ex " << ex <<std:: endl;
}
void Read() 
{

}
void Write() 
{

}
//void Run(char tmp, int size,rank)
//{
//	if (rank == 0) {
//		int pos;
//		do {
//			pos = FindFreeProcess(accessProcess, size - 1);
//			//	std::cout << pos << std::endl;
//		} while (pos == 0);
//		accessProcess[pos].lock();
//		//std::cout << "asdawd  " << accessProcess[pos].Info() << std::endl;
//	/*	for (int i = 0; i < size - 1; i++)
//			std::cout << accessProcess[i].Info() << std::endl;*/
//
//		MPI_Send(&tmp, 1, MPI_CHAR, pos, 0, MPI_COMM_WORLD);
//		MPI_Send(&pos, 1, MPI_INT, pos, 0, MPI_COMM_WORLD);
//		if (tmp == 'w')
//
//			//can smthg send
//			;
//	}
//
//}
//void Run( int rank) 
//{
//	MPI_Status status;
//	if (rank == 0) {
//		for (int i = 0; i < 4; i++)
//		{
//			MPI_Send(&readCount, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
//		}
//	}
//	if (rank != 0)
//	{
//		MPI_Recv(&readCount, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
//		readCount += 1;
//		std::cout << readCount << std::endl;
//	}
//}
void f(int a,int rank) 
{
	MPI_Request req;

	MPI_Ibcast(&a, 1, MPI_INT, rank, MPI_COMM_WORLD, &req);
}
void d(int a, int rank)
{
	MPI_Request req;
	MPI_Ibcast(&a, 1, MPI_INT, rank, MPI_COMM_WORLD, &req);
}
int main(int argc, char** argv) 
{
	int size; //количесво процессов
	int rank; //индентификатор процесса
	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int a = 0;
	if (rank == 1) {
		a = 5;
		f(a,rank);
	}
	if (rank == 2) {
		a = 10;
		d(a,rank);
	}
	
	std::cout << a << std::endl;

	MPI_Finalize();

	//int size; //количесво процессов
	//int rank; //индентификатор процесса
	//int pos; //position first freee process
	//char tmp;
	//int buf =5;
	//std::string str;
	//MPI_Init(&argc, &argv);
	//MPI_Status status;
	//MPI_Comm_size(MPI_COMM_WORLD, &size);
	//MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//std::ifstream file("..\\..\\Lab 2\\test.txt");

	//if (size <= 1) {
	//	std::cout << "Error count process" << std::endl;
	//	return -2;
	//}
	//accessProcess = new Mutex[size - 1];
	//
	//if (rank == 0)
	//{
	//	//std::ifstream file("D:\\GithubProjects\\parallel-programming-1\\1706-4\\dobrohotov_vn\\Lab 1\\Lab 2\\test.txt"/*, std::ios::in*/);
	//	if (file.is_open())
	//		while (!file.eof())
	//		{
	//			getline(file, str);
	//			std::cout << str << std::endl;
	//		}
	//	else {
	//		std::cout << "Error read file" << std::endl;
	//		return -3;
	//	}
	//	for (int i = 0; i < str.length(); i++)
	//	{
	//	
	//		Run(str[i], size);
	//	//	do {
	//	//		pos = FindFreeProcess(accessProcess, size - 1);
	//	//	//	std::cout << pos << std::endl;
	//	//	}
	//	//	while (pos == 0);
	//	//	accessProcess[pos].lock();
	//	//	//std::cout << "asdawd  " << accessProcess[pos].Info() << std::endl;
	//	///*	for (int i = 0; i < size - 1; i++)
	//	//		std::cout << accessProcess[i].Info() << std::endl;*/
	//	//	tmp = (char)str[i];
	//	//	MPI_Send(&tmp,1,MPI_CHAR,pos,0,MPI_COMM_WORLD);
	//	//	MPI_Send(&pos,1, MPI_INT, pos, 0, MPI_COMM_WORLD);
	//	//	if (tmp == 'w')

	//	//		//can smthg send
	//	//		;
	//	}
	//	
	//}

	//if (rank > 0)
	//{
	//	MPI_Recv(&tmp, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
	//	MPI_Recv(&pos, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	//	if (tmp == 'w')

	//		//can smthg get
	//		;
	//	Print(rank, accessProcess[pos].Info(), 0, tmp);
	//	//work with process
	//	accessProcess[pos].unlock();
	//	std::cout << "unlock  " << accessProcess[pos].Info() << std::endl;
	//	Sleep(5000);
	//}

	//if (rank==0)
	//	file.close();
	//
	//MPI_Finalize();
	return 0;
}