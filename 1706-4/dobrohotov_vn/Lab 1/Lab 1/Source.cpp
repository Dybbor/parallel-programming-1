#include <iostream>
#include <iomanip>
#include <mpi.h>

using namespace std;

int* InitMatrix(int rows, int cols) 
{
	int* vec = new int[rows*cols];
	if (rows <2 && cols < 2)
		for (int i = 0; i < rows*cols; i++)
			cin >> vec[i];
	else
		for (int i = 0; i < rows*cols; i++)
			vec[i] = rand() % 100 ;
	return vec;
}
void PrintMatrix(int *vec, int rows,int cols) 
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
	int* copy= new int[rows*cols];
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

int main(int argc, char** argv)
{

	int size;
	int rank;
	int rows, cols;
	double start_linear = 0, end_linear = 0,
		start_pp = 0, end_pp = 0;
	int*vec = NULL;
	int* res = NULL;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status Status;
	if (size < 1){
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
		vec = InitMatrix(rows, cols); //инциализируется матрица в виде вектора ранодомно
		res = new int[cols];	// массив в котором будут хранится максимальные значения
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
				res[k] = max;
				k++;
				max = vec[++i];
			}
		}
		end_linear = MPI_Wtime();

		//Паралельный алгоритм
		start_pp = MPI_Wtime();

		//0 процесс рассылает размеры матрицы и вектора и матрицу
		for (int i = 1; i < size; i++) {
			MPI_Send(&rows, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&cols, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(vec, rows*cols, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		//0 процесс собирает максимумы
		for (int i = 1; i < size; i++) {
			MPI_Recv(res, cols, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
			end_pp= MPI_Wtime();
		}
		delete[]vec;
		delete[]res;

	}
	else 
	{
		//процесс ожидает размеры матрицы и вектора от 0 процесса
		MPI_Recv(&rows, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
		MPI_Recv(&cols, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);


		vec = InitMatrix(rows, cols); //инциализируется матрица в виде вектора ранодомно
		res = new int[cols];	// массив в котором будут хранится максимальные значения

		//процесс ожидает от 0 процесса матрицу
		MPI_Recv(res, rows*cols, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);

		//поиск максимума по столбцам
		Tranpose(vec, rows, cols);
	

		//процесс отсылает результат 0 процессу
		MPI_Send(res, cols, MPI_INT, 0, 0, MPI_COMM_WORLD);

	}
	
	







	//	delete
	MPI_Finalize();
	system("pause");
	return 0;

}

