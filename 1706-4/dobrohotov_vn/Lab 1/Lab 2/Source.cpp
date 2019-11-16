#include <iostream>
#include "mpi.h"
#include <queue>
#include <windows.h>




const int READER_OPEN = 0;
const int READER_CLOSE = 1;
const int WRITER_OPEN = 2;
const int WRITER_CLOSE = 3;


const int EMPTY = -1;

const int RESPONSE_READER_YES = 4;
const int RESPONSE_READER_WAIT = 5;
const int RESPONSE_WRITER_YES = 6;
const int RESPONSE_WRITER_NO = 7;
const int RESPONSE_WRITER_WAIT = 8;

struct Request
{
	int rank;
	int step;
	int buf;

};

void RunReader(int rank,int time=1000) 
{
	MPI_Status status;
	Request request;
	int response;
	request.rank = rank;
	request.step = READER_OPEN;
	MPI_Send(&request, 3, MPI_INT, 0, 1, MPI_COMM_WORLD);
	MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	if (response == EMPTY) 
	{
		std::cout << "process " << rank << " - can't read" << std::endl;
	}
	else
	{
		if (response == RESPONSE_READER_WAIT)
		{
			MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			std::cout << "process " << rank << " - reader wait succeed" << std::endl;
		}
		std::cout << "process " << rank << " - reader open succeed" << std::endl;
		///
		Sleep(time);
		//////
		request.step = READER_CLOSE;
		MPI_Send(&request, 3, MPI_INT, 0, 1, MPI_COMM_WORLD);
		MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		std::cout << "process " << rank << " - reader finish" << std::endl;
	}
}

void RunWriter(int rank,int time=1000) 
{
	MPI_Status status;
	Request request;
	int response;
	request.rank = rank;
	request.step = WRITER_OPEN;
	MPI_Send(&request, 3, MPI_INT, 0, 1, MPI_COMM_WORLD);
	MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	if (response == RESPONSE_WRITER_WAIT) 
	{
		MPI_Recv(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		std::cout << "process " << rank << " - writer wait succeed" << std::endl;
	}
	std::cout << "process " << rank << " - writer open succeed" << std::endl;
	////
	Sleep(time);
	//////
	request.step = WRITER_CLOSE;
	request.buf=1;
	MPI_Send(&request, 3, MPI_INT, 0, 1, MPI_COMM_WORLD);
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
	if (size < 3) 
	{
		std::cout << "Need  minimum 3 process" << std::endl;
		MPI_Finalize();
		return -1;
	}	
	if (rank == 0)
	{
		int buf = 0;
		int countOfReaders=0;
		int countOfWriters=0;
		std::queue <int> q_readers,q_writers;
		bool waitingWriter = false;
		int numb = 0;
		for (int i = 0; i < 8/*(size-4)*k*2 +16*//*12*/; i++) 
		{
			MPI_Status status;
			Request request;
			MPI_Recv(&request, 3, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			std::cout << "\nStatus" <<std:: endl;
			std::cout << "buf " << buf << std::endl;
			std::cout << "reader " << countOfReaders << " writer " << countOfWriters << std::endl;
			std::cout << "wait reader size " << q_readers.size() << "  wait writer size " << q_writers.size() << std::endl;
			std::cout << "\n\t\t "<<numb<<" NEW REQUEST" << std::endl;
			numb++;
			switch (request.step)
			{
			case READER_OPEN:
			{
				if (buf == 0) 
				{
					std::cout << "\t\t  Manager: process " << request.rank << " read - buf empty" << std::endl;
					MPI_Send(&EMPTY, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
					
				}
				else if (countOfWriters == 0)
				{
					countOfReaders++;
					std::cout << "\t\t  Manager: process " <<request.rank<< " open reader - yes\n" << std::endl;
					MPI_Send(&RESPONSE_READER_YES, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
				}
				else 
				{
					std::cout << " \t\t Manager: process " << request.rank << " open reader - no\n" << std::endl;
					q_readers.push(request.rank);
					MPI_Send(&RESPONSE_READER_WAIT, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
				}
				break;
			}
			case READER_CLOSE:
			{
					countOfReaders--;
					std::cout << " \t\t Manager: process " << request.rank << " close reader - yes\n" << std::endl;
					MPI_Send(&RESPONSE_READER_YES, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
					if (q_writers.size() > 0 && countOfReaders==0) 
					{
						std::cout << "\t\t \nQueue writer start\n" << std::endl;
						//for (int i = 0; i < q_writers.size(); i++)
						//{
							countOfWriters++;
							MPI_Send(&RESPONSE_WRITER_YES, 1, MPI_INT, q_writers.front(), 0, MPI_COMM_WORLD);
							q_writers.pop();
						//}
					}
				break;
			}
			case WRITER_OPEN:
			{
				if (countOfReaders == 0 && countOfWriters==0)
				{
					countOfWriters++;
					std::cout << "\t\t  Manager: process " << request.rank << " open writer - yes\n" << std::endl;
					MPI_Send(&RESPONSE_WRITER_YES, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
				}
				else 
				{
					std::cout << "\t\t  Manager: process " << request.rank << " open writer - no\n" << std::endl;
					q_writers.push(request.rank);
					MPI_Send(&RESPONSE_WRITER_WAIT, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);
				}
				break;
			}
			case WRITER_CLOSE:
			{
				countOfWriters--;
				buf +=request.buf;
				std::cout << "\t\t  Manager: process " << request.rank << " close writer - yes\n" << std::endl;
				MPI_Send(&RESPONSE_WRITER_YES, 1, MPI_INT, request.rank, 0, MPI_COMM_WORLD);

				if (q_writers.size() > 0 && countOfReaders == 0)
				{
					std::cout << "\nQueue writer start\n" << std::endl;
					//for (int i = 0; i < q_writers.size(); i++)
					//{
						countOfWriters++;
						MPI_Send(&RESPONSE_WRITER_YES, 1, MPI_INT, q_writers.front(), 0, MPI_COMM_WORLD);
						q_writers.pop();
					//}
				}
				else if (q_readers.size() > 0)
				{
					std::cout << "\nQueue reader start\n" << std::endl;
					int q_size = q_readers.size();
					for (int i = 0; i < q_size; i++)
					{
						countOfReaders++;
						MPI_Send(&RESPONSE_READER_YES, 1, MPI_INT, q_readers.front(), 0, MPI_COMM_WORLD);
						q_readers.pop();
					}
				}
				break;
			}
			}
		}
	}
	if (rank == 1) 
	{
		RunWriter(rank, 2000);
		RunWriter(rank, 1000);

	}
	if (rank == 2) 
	{
		RunReader(rank, 500);
		RunWriter(rank, 200);
		RunReader(rank, 500);
		
	}
	if (rank == 3)
	{
		RunWriter(rank, 200);
		RunWriter(rank, 1000);
		RunReader(rank, 500);
	}
	if (rank>3 && rank % 2 ==0 && rank!=0)
	{
		for (int i = 0; i < k; i++) 
		{
			RunReader(rank);
		}
	}
	if (rank>3 && rank % 2 == 1) 
	{
		for (int i = 0; i < k; i++) 
		{
			RunWriter(rank);
		}
	}
	MPI_Finalize();
	return 0;
}