#include <iostream>
#include <iomanip>
#include "mpi.h"

void PrintInfo(int rows, int cols, int block,int left, int size, int rank)
{
	std::cout << "rows " << rows << "\ncols " << cols << "\nblock " << block <<"\nleft "<<left<< "\nsize " << size << "\nrank " << rank << std::endl;
}
double* InitMatrix(int rows, int cols)
{
	double* vec = new double[rows*cols];
	if (rows < 2 && cols < 2)
		for (int i = 0; i < rows*cols; i++)
			std::cin >> vec[i];
	else
		for (int i = 0; i < rows*cols; i++)
			vec[i] = (rand() % RAND_MAX)/*/100.0*/;
	return vec;
}

void PrintMatrix(double *vec, int rows, int cols)
{
	if (rows < 100 && cols < 100) {
		std::cout << vec[0] << std::setw(2) << "  ";
		for (int i = 1; i < rows*cols; i++)
		{
			std::cout << std::setw(2) << double(vec[i]);
			(i % (cols)) == cols - 1 ? std::cout << std::endl : std::cout << "  ";
		}
		std::cout << std::endl;
	}
	else
		std::cout << "Matrix too large" <<std::endl;
}
int PrintVector(double *x, int size)
{
	if (size < 50)
		for (int i = 0; i < size; i++)
			std::cout << x[i] << "  ";
	else
		std::cout << "Vector too large" << std::endl;
	return 1;
}
void FindMax(double *vec, double * res, int cols, int block)
{
	int max;
	for (int i = 0; i < cols; i++)
	{
		max = vec[i];
		for (int j = 0; j < block; j++)
		{
			if (max < vec[i + j * cols])
				max = vec[i + j * cols];
		}
		res[i] = max;
	}
}

int main(int argc, char **argv) 
{
	int size; //количесво процессов
	int rank; //индентификатор процесса
	int rows, cols; //строки колонки
	double start_linear = 0, end_linear = 0,
		start_pp = 0, end_pp = 0;
	double* vec = NULL; //Матрица в виде вектора
	double* resl = NULL; //Результирующий вектор максимумов для последовательного
	double *resp;//=NULL; //= NULL; //Результирующий вектор максимумов для параллейного
	double *tmp = NULL; //Локальный массив для блока
	double *local_max = NULL; //здесь будут храниться локальные максимумы
	double *tmp_max = NULL; //Промежуточные максимумы + строки не попавшие на процессы
	int block; //Количество столбцов в блоке
	int left; //Остаточные блоки 
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (size < 1) {
		std::cout << "Error count process"<<std::endl; 
		return -2;
	}
	if (rank == 0) {
		while (1) {
			std::cout << "Input rows and cols" << std::endl;
			std::cin >> rows;
			std::cin >> cols;
			if (cols < 1 || rows < 1)
			{
				system("cls");
				std::cout << "rows and cols must be >0" << std::endl;
			}
			else
				break;
		}

		//Initialize
		if (cols < size) //Если колонок меньше чем процессов, не задействовать все процессы, а столько сколько нужно
			size = cols;
		
		vec = InitMatrix(rows, cols);
		block = rows / size; //в блоке n-ое кол во столбцов
		left = rows % size;  //остаточные столбцы которые не попадут на процессы
		resl = new double[cols];


		//Linear algorithm
		start_linear = MPI_Wtime();
		FindMax(vec, resl, cols, rows);
		end_linear = MPI_Wtime();
	}
	if (rank>0)
		if (cols < size) //Если колонок меньше чем процессов, не задействовать все процессы, а столько сколько нужно
			size = cols;
	//parellel algorithm
	start_pp = MPI_Wtime();
	MPI_Bcast(&block, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

	tmp = new double[cols*block];
	local_max = new double[cols];
	resp = new double[cols];	// массив в котором будут хранится максимальные значения
	tmp_max = new double[cols*block+left*cols];

	MPI_Scatter(vec, cols*block, MPI_DOUBLE, tmp, cols*block, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	FindMax(tmp, local_max, cols, block);
	MPI_Gather(local_max, cols, MPI_DOUBLE, tmp_max, cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	if (rank == 0)
		if (left != 0)
		{
			int pos = size * cols;
			for (int i = block * size*cols; i < rows*cols; i++) {
				tmp_max[pos] = vec[i];
				pos++;
			}

		}
	FindMax(tmp_max, resp,cols, (size+left));
	end_pp = MPI_Wtime();
	if (rank == 0)
	{
		PrintMatrix(vec, rows, cols);
		std::cout << "\n" << std::endl;
		PrintVector(resl, cols);
		std::cout << std::endl;
		PrintVector(resp, cols);
		std::cout << "\n" << std::endl;
		std::cout << "linear time  " << (end_linear - start_linear) * 1000 << std::endl;
		std::cout << "pp time  " << (end_pp - start_pp) * 1000 << std::endl;
	}
	/*PrintInfo(rows, cols, block,left, size, rank);
	if (rank==0)
		std::cout << "Process " << rank << "\n" << PrintVector(tmp_max, size*cols + left * cols) << std::endl;
	if (rank == 0)
		std::cout << "res " << rank << "\n" << PrintVector(resp, cols) << std::endl;*/
	delete[]vec;
	delete[]resl;
	delete[]resp;
	delete[] tmp;
	delete[] local_max;
	delete[] tmp_max;
	MPI_Finalize();
	return 0;
}