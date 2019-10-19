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
void PrintInfo(int rank, int size, int block, int rows, int cols, int *tmp, int *local_max)
{
	cout << "\nInformation\nProcess " << rank << "\n" << "Size " << size << "\n" << "block " << block << "\n" << "rows " << rows << "\n" << "cols " << cols << endl;

	for (int i = 0; i < rows*block; i++)
		cout << " tmp " << rank << " " << tmp[i] << " ";
	for (int i = 0; i < block; i++)
		cout << "local max " << rank << " " << local_max[i] << " ";
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
void Tranpose(int* vec, int rows, int cols)
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

int main(int argc, char ** argv)
{
	int size; //��������� ���������
	int rank; //�������������� ��������
	int rows, cols; //������� �������
	double start_linear = 0, end_linear = 0,
		start_pp = 0, end_pp = 0;
	int* vec = NULL; //������� � ���� �������
	int* resl = NULL; //�������������� ������ ���������� ��� �����������������
	int *resp;//=NULL; //= NULL; //�������������� ������ ���������� ��� ������������
	int *tmp = NULL; //��������� ������ ��� �����
	int *local_max = NULL; //����� ����� ��������� ��������� ���������
	int block; //���������� �������� � �����
	int left; //���������� ����� 
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
		if (cols < size) //���� ������� ������ ��� ���������, �� ������������� ��� ��������, � ������� ������� �����
			size = cols;
		vec = InitMatrix(rows, cols); //��������������� ������� � ���� ������� ��������
		block = cols / size; //� ����� n-�� ��� �� ��������
		left = cols % size;  //���������� ������� ������� �� ������� �� ��������
		tmp = new int[rows*block];	// ������������� ������ ��� ������ �������
		local_max = new int[block];
		resl = new int[cols];	// ������ � ������� ����� �������� ������������ ��������
		//resp = new int[cols];	// ������ � ������� ����� �������� ������������ ��������
		/*resp = new vector <int>();
		resp->resize(cols);*/
		PrintMatrix(vec, rows, cols);
		Tranpose(vec, rows, cols); //���������������� ������� (������������� �������)
	}
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&block, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
	resp = new int[cols];	// ������ � ������� ����� �������� ������������ ��������
	if (rank > 0)
	{
		tmp = new int[rows*block];	// ������������� ������ ��� ������ �������
		//resp = new int[cols];	// ������ � ������� ����� �������� ������������ ��������
		/*resp = new vector <int>();
		resp->resize(cols);*/
		local_max = new int[block];
	}
	MPI_Scatter(vec, rows*block, MPI_INT, tmp, rows*block, MPI_INT, 0, MPI_COMM_WORLD);
	//����� ��������� �� �������� � ��������� ��� � ������ ��������� ����
	int c = 0; //��������
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
	PrintInfo(rank, size, block, rows, cols, tmp, local_max);

	MPI_Gather(local_max, block, MPI_INT, resp, block, MPI_INT, 0, MPI_COMM_WORLD);
	if (rank == 0)
		for (int i = 0; i < cols; i++)
			cout << " max vector" << resp[i] << " ";
	//��������� ������� ������� �� ������ �� ��������
	if (rank == 0)
		if (left != 0)
		{
			int c = 0; //��������
			for (int i = 0; i < left; i++)
			{
				int max = vec[rows*size + c];
				for (int j = 1; j < rows; j++)
				{
					if (vec[j + rows * size + c] > max)
						max = vec[j + rows * size + c];
				}
				c += rows;
				resp[i + size] = max;
			}
		}
	if (rank == 0)
	{
		cout << "\nafter" << endl;
		for (int i = 0; i < cols; i++)
			cout << " max vector" << resp[i] << " ";
	}
	//MPI_Barrier(MPI_COMM_WORLD);

	/*cout << "Process " << rank << endl;
	cout << " Size " << size << endl;
	cout << "Block " << block;*/
	delete[]vec;
	delete[]resl;
	//delete[]resp;

	MPI_Finalize();
}