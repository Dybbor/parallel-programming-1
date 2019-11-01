#include <iostream>
#include <iomanip>
#include <mpi.h>

using namespace std;

double* InitMatrix(int rows, int cols)
{
	double* vec = new double[rows*cols];
	if (rows < 2 && cols < 2)
		for (int i = 0; i < rows*cols; i++)
			cin >> vec[i];
	else
		for (int i = 0; i < rows*cols; i++)
			vec[i] = rand() % 10000;
	return vec;
}
void PrintVector(double *x, int size)
{
	if (size < 50)
		for (int i = 0; i < size; i++)
			cout << x[i] << "  ";
	else
		cout << "Vector too large" << endl;
}
//void PrintInfo(int rank, int size, int block, int rows, int cols, int *tmp, int *local_max)
//{
//	cout << "\nInformation\nProcess " << rank << "\n" << "Size " << size << "\n" << "block " << block << "\n" << "rows " << rows << "\n" << "cols " << cols << endl;
//
//	for (int i = 0; i < rows*block; i++)
//		cout << " tmp " << rank << " " << tmp[i] << " ";
//	for (int i = 0; i < block; i++)
//		cout << "local max " << rank << " " << local_max[i] << " ";
//}
void PrintMatrix(double *vec, int rows, int cols)
{
	if (rows < 100 && cols < 100) {
		cout << vec[0] << setw(2) << "  ";
		for (int i = 1; i < rows*cols; i++)
		{
			cout << setw(2) << vec[i];
			(i % (cols)) == cols - 1 ? cout << endl : cout << "  ";
		}
		cout << endl;
	}
	else
		cout << "Matrix too large" << endl;
}
void Tranpose(double* vec, int rows, int cols)
{
	double* copy = new double[rows*cols];
	for (int i = 0; i < rows*cols; i++)
		copy[i] = vec[i];
	int ind = 0;
	for (int i = 0; i < cols; i++) {
		int n = 0;
		for (int j = 0; j < rows; j++) {
			vec[ind] = copy[i + n];
			ind++;
			n += cols;
		}
	}
	delete[] copy;
}

//void Max(int *mas, int *res, int cols, int pos)
//{
//	int max = mas[0];
//	for (int i = 1; i < cols; i++)
//	{
//		if (max < mas[i])
//			max = mas[i];
//	}
//	res[pos] = max;
//}

int main(int argc, char ** argv)
{
	int size; //количесво процессов
	int rank; //индентификатор процесса
	int rows, cols; //строкиб колонки
	double start_linear = 0, end_linear = 0,
		start_pp = 0, end_pp = 0;
	double* vec = NULL; //Матрица в виде вектора
	double* resl = NULL; //Результирующий вектор максимумов для последовательного
	double *resp;//=NULL; //= NULL; //Результирующий вектор максимумов для параллейного
	double *tmp = NULL; //Локальный массив для блока
	double *local_max = NULL; //здесь будут храниться локальные максимумы
	int block; //Количество столбцов в блоке
	int left; //Остаточные блоки 
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (size < 1) {
		cout << "Error count process" << endl;
		return -2;
	}
	if (rank == 0) {
		//Input Matrix
		while (1) {
			cout << "Input rows and cols" << endl;
			cin >> rows;
			cin >> cols;
			if (cols < 1 || rows < 1)
			{
				system("cls");
				cout << "rows and cols must be >0" << endl;
			}
			else
				break;
		}
		//Initialize
		if (cols < size) //Если колонок меньше чем процессов, не задействовать все процессы, а столько сколько нужно
			size = cols;
		vec = InitMatrix(rows, cols); //инциализируется матрица в виде вектора рандомно
		PrintMatrix(vec, rows, cols);
		Tranpose(vec, rows, cols); //Транспонирование матрицы (переделывание вектора)
		block = cols / size; //в блоке n-ое кол во столбцов
		left = cols % size;  //остаточные столбцы которые не попадут на процессы
		tmp = new double[rows*block];	// промежуточный массив для частей вектора
		local_max = new double[block];

		//Linear algorithm
		resl = new double[cols];	// массив в котором будут хранится максимальные значения линейного алгоритма
		start_linear = MPI_Wtime();
		int max = vec[0];
		int k = 0;
		for (int i = 1; i < rows*cols; i++)
		{
			if (max < vec[i])
				max = vec[i];
			if (i % rows == rows - 1) {
				resl[k] = max;
				k++;
				max = vec[++i];
			}
		}
		end_linear = MPI_Wtime();
	}
	//parellel algorithm
	start_pp = MPI_Wtime();
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&block, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
	resp = new double[cols];	// массив в котором будут хранится максимальные значения
	if (rank > 0)
	{
		tmp = new double[rows*block];	// промежуточный массив для частей вектора
		local_max = new double[block];
	}
	MPI_Scatter(vec, rows*block, MPI_DOUBLE, tmp, rows*block, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//Поиск максимума по столбцам и занесение его в массив локальных сумм
	int c = 0; //Смещение
	for (int i = 0; i < block; i++)
	{
		int max = tmp[0 + c];
		for (int j = 1; j < rows; j++)
		{
			if (tmp[j + c] > max)
				max = tmp[j + c];
		}
		c += rows;
		local_max[i] = max;
	}
	MPI_Gather(local_max, block, MPI_DOUBLE, resp, block, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//Считаются столбцы которые не попали на процессы
	if (rank == 0)
		if (left != 0)
		{
			cout << "i am in left" << endl;
			int c = 0; //Смещение
			for (int i = 0; i < left; i++)
			{
				int max = vec[rows*size*block + c];
				for (int j = 1; j < rows; j++)
				{
					if (vec[j + rows * size * block + c] > max)
						max = vec[j + rows * size *block + c];
				}
				c += rows;
				resp[i + size*block] = max;
			}
		}
	end_pp = MPI_Wtime();
	if (rank == 0) 
	{
		PrintVector(resl,cols);
		cout << endl;
		PrintVector(resp, cols);
		cout << endl;
		cout << "time linear  " << (end_linear - start_linear) * 1000 << endl;
		cout << "time parallel  " << (end_pp - start_pp) * 1000 << endl;
	}
	delete[]vec;
	delete[]resl;
	//delete[]resp;
	delete[] tmp;
	delete[] local_max;
	MPI_Finalize();
}