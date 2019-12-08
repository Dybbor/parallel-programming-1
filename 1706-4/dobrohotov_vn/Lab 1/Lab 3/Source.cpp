//#include <iostream>
//#include <opencv2/opencv.hpp>
//#include "mpi.h"
//#include <cmath>
//using namespace cv;
//using namespace std;
//double** createGauseKerel(double **kernel,double sigma)
//{
//	int radius = 1;
//	double norm = 0;
//	for (int i = -radius; i <=radius; ++i) 
//	{
//		for (int j = -radius; j <= radius; ++j)
//		{
//			kernel[i + radius][j + radius] = (double)(exp(-(i*i + j * j) / sigma * sigma));
//			norm += kernel[i + radius][j + radius];
//		}
//	}
//	cout <<"Norm " <<norm << endl;
//	for (int i = 0; i < 3; i++)
//		for (int j = 0; j < 3; j++)
//			kernel[i][j] /= norm;
//	return kernel;
//}
//
//
//int main(int argc,char**argv) 
//{
//	//Initialize
//	int rank, size;
//	Mat image;
//	//Start program
//	MPI_Init(&argc, &argv);
//	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
//	MPI_Comm_size(MPI_COMM_WORLD, &size);
//	if (size == 1) {
//		cout << "Need more process then 1" << endl;
//		MPI_Finalize();
//		return -1;
//	}
//	if (rank == 0) 
//	{
//		image = imread("D:\\GitProject\\parallel-programming-1\\1706-4\\dobrohotov_vn\\Lab 1\\Picture\\cat.jpg");
//		namedWindow("Image", WINDOW_NORMAL);
//		if (!image.data) {
//			cout << "Error load image" << endl;
//			MPI_Finalize();
//			return - 2;
//		}
//		if (image.channels()>1) 
//		{
//			cvtColor(image, image, COLOR_BGR2GRAY);
//			cout << "image to grey color" << endl;
//		}
//	}
//	MPI_Finalize();
//	imshow("Image", image);
//	waitKey();
//	return 0;
//}