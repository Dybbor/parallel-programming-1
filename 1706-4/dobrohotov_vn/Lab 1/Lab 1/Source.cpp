#include <iostream>
#include <iomanip>
#include <mpi.h>

using namespace std;

int* InitMatrix(int rows, int cols)
{
	int* vec = new int[rows*cols];
	if (rows < 2 && cols < 2)
		for (int i = 0; i < rows*cols; i++)
			cin >> vec[i];
	else
		for (int i = 0; i < rows*cols; i++)
			vec[i] = rand() % 10000;
	return vec;
}
void PrintVector(int *x, int size)
{
	for (int i = 0; i < size; i++)
		cout << x[i] << "  ";
}
void PrintMatrix(int *vec, int rows, int cols)
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
		cout << "Matrix too big" << endl;
}
void Tranpose(int* vec, int &rows, int &cols)
{
	int* copy = new int[rows*cols];
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

void Max(int *mas, int *res, int cols, int pos)
{
	int max = mas[0];
	for (int i = 1; i < cols; i++)
	{
		if (max < mas[i])
			max = mas[i];
	}
	res[pos] = max;
}


int main(int argc, char** argv)
{

	int size; //количесво процессов
	int rank; //индентификатор процесса
	int rows, cols; //строкиб колонки
	double start_linear = 0, end_linear = 0,
		start_pp = 0, end_pp = 0;
	int* vec = NULL; //Матрица в виде вектора
	int* resl = NULL; //Результирующий вектор максимумов для последовательного
	int* resp = NULL; //Результирующий вектор максимумов для параллейного
	int *tmp = NULL; //Локальный массив для блока
	int *local_max = NULL; //здесь будут храниться локальные максимумы
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
		vec = InitMatrix(rows, cols); //инциализируется матрица в виде вектора рандомно
		block = cols / size; //в блоке n-ое кол во столбцов
		tmp = new int[cols*block];	// промежуточный массив для частей вектора
		local_max = new int[block];
		resl = new int[cols];	// массив в котором будут хранится максимальные значения
		resp = new int[cols];	// массив в котором будут хранится максимальные значения
		left = cols % size;  //остаточные столбцы которые не попадут на процессы
		Tranpose(vec, rows, cols); //Транспонирование матрицы (переделывание вектора)
		//Последовательный алгоритм
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
		PrintVector(resl, cols);
		//Паралельный алгоритм
		start_pp = MPI_Wtime();
		//MPI_Bcast(&block, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Scatter(vec, cols*block, MPI_INT, tmp, cols*block, MPI_INT, 0, MPI_COMM_WORLD);
		int c = 0;
		for (int i = 0; i < block; i++) {
			Max(tmp + c, local_max, cols, i);
			c += cols;
		}
	}
	else
	{
		cols = 0;
		block = 0;
		tmp = new int[cols*block];	// промежуточный массив для частей вектора
		resp = new int[cols];	// массив в котором будут хранится максимальные значения
		local_max = new int[block];
		int c = 0;
		for (int i = 0; i < block; i++) {
			Max(tmp + c, local_max, cols, i);
			c += cols;
		}
	}
	if (rank == 0)
	{
		MPI_Gather(local_max, 3, MPI_INT, resp, 3, MPI_INT, 0, MPI_COMM_WORLD);
		if (left != 0)
			for (int i = 0; i < left; i++)
			{

			}
		cout << "PP" << endl;
		PrintVector(resp, cols);
	}
	end_pp = MPI_Wtime();

	delete[]vec;
	delete[]resl;
	delete[]resp;
	MPI_Finalize();
	return 0;

}

