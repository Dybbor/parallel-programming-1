#include <iostream>
#include "mpi.h"
#include <queue>
#include <windows.h>
using namespace std;


const int READER_OPEN = 0;
const int READER_CLOSE = 1;
const int WRITER_OPEN = 2;
const int WRITER_CLOSE = 3;

const int RESPONCE_READER_YES = 4;
const int RESPONCE_READER_WAIT = 5;
const int RESPONCE_WRITER_YES = 6;
const int RESPONCE_WRITER_NO = 7;
const int RESPONCE_WRITER_WAIT = 8;


struct Request
{
	int rank;
	int step;
};


void RunReader(int rank) 
{
	MPI_Status status;
	Request request;
	int response;
	request.rank = rank;
	request.step = READER_OPEN;
	MPI_Send(&request, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
	MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	if (response == RESPONCE_READER_WAIT)
	{
		MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		std::cout << "process " << rank << " - reader wait succeed" << std::endl;
	}
	std::cout << "process " << rank<< " - reader open succeed" << std::endl;
	///
	Sleep(1000);
	//////
	request.step = READER_CLOSE;
	MPI_Send(&request, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
	MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	std::cout << "process " << rank << " - reader finish" << std::endl;
}

void RunWriter(int rank) 
{
	MPI_Status status;
	Request request;
	int response;
	request.rank = rank;
	request.step = WRITER_OPEN;
	MPI_Send(&request, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
	MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	if (response == RESPONCE_WRITER_WAIT) 
	{
		MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		std::cout << "process " << rank << " - writer wait succeed" << std::endl;
	}
	std::cout << "process " << rank << " - writer open succeed" << std::endl;
	////
	Sleep(1000);
	//////

	request.step = WRITER_CLOSE;
	MPI_Send(&request, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
	MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	std::cout << "process " << rank << " - write finish" << std::endl;
}
int main(int argc, char**argv) 
{
	int rank;
	int size;
	const int k = 2;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	if (rank == 0)
	{
		int countOfReaders=0;
		int countOfWriters=0;
		queue <int> q_readers,q_writers;
		bool waitingWriter = false;
		for (int i = 0; i < k * 2; i++) 
		{
			MPI_Status status;
			Request request;
			MPI_Recv(&request, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			switch (request.step)
			{
			case READER_OPEN:
			{
				if (countOfWriters == 0)
				{
					countOfReaders++;
					std::cout << " Manager: process " <<request.rank<< " open reader - yes" << std::endl;
					MPI_Send(&RESPONCE_READER_YES, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
				}
				else 
				{
					std::cout << " Manager: process " << request.rank << " open reader - no" << std::endl;
					q_readers.push(request.rank);
					MPI_Send(&RESPONCE_READER_WAIT, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
				}
				break;
			}
			case READER_CLOSE:
			{
					countOfReaders--;
					std::cout << " Manager: process " << request.rank << " close reader - yes" << std::endl;
					MPI_Send(&RESPONCE_READER_YES, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
					if (q_writers.size() > 0 && countOfReaders==0) 
					{
						for (int i = 0; i < q_writers.size(); i++)
						{
							MPI_Send(&RESPONCE_WRITER_YES, 1, MPI_INT, q_writers.front(), 0, MPI_COMM_WORLD);
							q_writers.pop();
						}
					}
				break;
			}
			case WRITER_OPEN:
			{
				if (countOfReaders == 0 && countOfWriters==0)
				{
					countOfWriters++;
					std::cout << " Manager: process " << request.rank << " open writer - yes" << std::endl;
					MPI_Send(&RESPONCE_WRITER_YES, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
				}
				else 
				{
					std::cout << " Manager: process " << request.rank << " open writer - no" << std::endl;
					q_writers.push(request.rank);
					MPI_Send(&RESPONCE_WRITER_WAIT, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
				}
				break;
			}
			case WRITER_CLOSE:
			{
				countOfWriters--;
				std::cout << " Manager: process " << request.rank << " close writer - yes" << std::endl;
				MPI_Send(&RESPONCE_WRITER_YES, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
				if (q_readers.size() > 0)
				{
					for (int i = 0; i < q_readers.size(); i++) 
					{
						MPI_Send(&RESPONCE_READER_YES, 1, MPI_INT, q_readers.front(), 0, MPI_COMM_WORLD);
						q_readers.pop();
					}
				}
				break;
			}
			}
		}
	}
	if (rank % 2 ==0 && rank!=0)
	{
		for (int i = 0; i < k; i++) 
		{
			RunReader(rank);
		}
	}
	if (rank % 2 == 1) 
	{
		for (int i = 0; i < k; i++) 
		{
			RunWriter(rank);
		}
	}
	MPI_Finalize();
	return 0;
}