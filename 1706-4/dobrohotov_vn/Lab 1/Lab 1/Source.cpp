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

int main(int argc,char** argv) 
{
	MPI_Init(&argc, &argv);
	int rows, cols;
	double start_linear=0, end_linear=0;
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
	int *vec = InitMatrix(rows, cols);
	int *res = new int[cols];
	PrintMatrix(vec, rows, cols);
	/*cout << endl;
	for (int i = 0; i < rows*cols; i++)
		cout << vec[i] << "   ";
	cout << endl;*/
	Tranpose(vec, rows, cols);
	start_linear = MPI_Wtime();
	int max = vec[0];
	int k = 0;
	for (int i = 1; i < rows*cols;i++) 
	{
		if (max < vec[i])
			max = vec[i];
		if (i % rows == rows-1) {
			res[k] = max;
			k++;
			max = vec[++i];
		}
	}
	end_linear = MPI_Wtime();
	PrintMatrix(vec, cols, rows);
	/*cout << endl;
	for (int i = 0; i < rows*cols; i++)
		cout << vec[i] << "   ";
	cout << endl;
	cout << endl;
	cout << endl;*/
	/*for (int i = 0; i < cols; i++)
		cout << res[i] << "  ";*/
	cout << "time linear   " << (end_linear - start_linear) << endl;
	delete[]vec;
	delete[]res;
	MPI_Finalize();
	system("pause");
	return 0;

}

