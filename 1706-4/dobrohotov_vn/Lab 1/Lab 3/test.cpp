#include <iostream>
#include <cmath>
double** CreateGaussianBlur(double** kernel,double sigma) 
{
	int norm = 0;
	for (int i = -1; i < 2; ++i)
		for (int j = -1; j < 2; j++) 
		{
			kernel[i + 1][j + 1] = (float)(exp(-(i*i + j * j) / (sigma*sigma)));
			norm += kernel[i][j]; 
		}
	
	return kernel;
}

void main() 
{
	double** mas= new double*[3];
	for (int i = 0; i < 3; ++i)
		mas[i] = new double[3];
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; j++)
			mas[i][j] = 0;
	mas = CreateGaussianBlur(mas,2);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; j++)
		{
			std::cout <<mas[i][j]*4 << " ";
		}
		std::cout << std::endl;
	}
	system("pause");
}