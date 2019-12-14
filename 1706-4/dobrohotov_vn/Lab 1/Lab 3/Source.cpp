#include <iostream>
#include <opencv2/opencv.hpp>
#include "mpi.h"
#include <cmath>
using namespace cv;
using namespace std;

#define M_PI 3.14159265358979323846
double* createKernel(double* kernel, double sigma)
{
	double sum = 0;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			kernel[i + j + 2 * (i + 2)] = (double)(exp(-(i*i + j * j) / (sigma*sigma))) *(1 / (sigma*sqrt(2 * M_PI)));
			sum += kernel[i + j + 2 * (i + 2)];
		}
	}
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			kernel[i + j + 2 * (i + 2)] /= sum;
		}
	}
	return kernel;
}

Mat duplicateBorder(Mat image)
{

	Mat new_image(image.rows + 2, image.cols + 2, CV_8UC1);
	for (int i = 0; i < new_image.rows; i++)
		for (int j = 0; j < new_image.cols; j++)
			new_image.at<uchar>(i, j) = 128;
	new_image.at<uchar>(0, 0) = image.at<uchar>(0, 0);
	new_image.at<uchar>(0, new_image.cols - 1) = image.at<uchar>(0, image.cols - 1);
	new_image.at<uchar>(new_image.rows - 1, 0) = image.at<uchar>(image.rows - 1, 0);
	new_image.at<uchar>(new_image.rows - 1, new_image.cols - 1) = image.at<uchar>(image.rows - 1, image.cols - 1);
	for (int i = 0; i < image.cols; i++)
	{
		new_image.at<uchar>(0, i + 1) = image.at<uchar>(0, i);
		new_image.at<uchar>(new_image.rows - 1, i + 1) = image.at<uchar>(image.rows - 1, i);
	}
	for (int i = 0; i < image.rows; i++)
	{
		new_image.at<uchar>(i + 1, 0) = image.at<uchar>(i, 0);
		new_image.at<uchar>(i + 1, new_image.cols - 1) = image.at<uchar>(i, image.cols - 1);
	}
	for (int i = 0; i < image.rows; i++)
		for (int j = 0; j < image.cols; j++)
		{
			new_image.at<uchar>(i + 1, j + 1) = image.at<uchar>(i, j);
		}

	return new_image;
}
uchar*  processImage(uchar* data, int rows, int cols)
{
	uchar *res_data = new uchar[(rows - 2)*(cols - 2)];
	double kernel[9] = { 1,2,1,2,4,2,1,2,1 };
	//normirovvka intensity
	for (int i = 0; i < 9; ++i)
		kernel[i] /= 16;
	int count_res_data = 0;
	int move = 0;
	//cout << (rows)*(cols)-2 * (cols)-1 << " ind" << endl;
	for (int i = 0; i < ((rows)*(cols)-2 * (cols)-1); i++)
	{
		int res = 0;
		move = 0;
		//cout << i;
		int count_kernel = 0;
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				//cout <<"   "<<(int)new_image.data[i + j + k + move] << " ";
				res += data[i + j + k + move] * kernel[count_kernel];
				count_kernel++;
			}
			move += (cols)-1;
		}
		if (res > 255)
			res = 255;
		if (res < 0)
			res = 0;
		res_data[count_res_data] = res;
		count_res_data++;
		cout << "asd" << " " << i << endl; 
		if ((i + 3) % (cols) == 0)
			i += 2;
		
	}
	return res_data;
}


int main(int argc, char**argv)
{
	//Initialize
	int rank, size;
	Mat image, copy, res_linear, res_pp;
	uchar* data = nullptr;
	uchar* tmp = nullptr;
	uchar *local_res = nullptr;
	double start_linear;
	double finish_linear;
	double start_pp;
	double finish_pp;
	double start_duplicate;
	double finish_duplicate;
	int block;
	int left;
	int rows;
	int cols;
	int *count = nullptr;
	int *displs = nullptr;
	int sent_position_row;
	//Start program
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size == 1) {
		cout << "Need more process then 1" << endl;
		MPI_Finalize();
		return -1;
	}
	if (rank == 0)
	{
		image = imread("D:\\GitProject\\parallel-programming-1\\1706-4\\dobrohotov_vn\\Lab 1\\Picture\\test1.jpg");
		if (!image.data) {
			cout << "Error load image" << endl;
			MPI_Finalize();
			return -2;
		}
		if (image.channels() > 1)
		{
			cvtColor(image, image, COLOR_BGR2GRAY);
			cout << "image to grey color" << endl;
		}
		//Initialize image
		res_linear = image.clone();
		res_pp = image.clone();
		/*	block = image.rows / size;
			left = image.cols % size;*/
		rows = image.rows;
		cols = image.cols;
		data = new uchar[image.cols*image.rows];
		//dublicate
		start_duplicate = MPI_Wtime();
		copy = duplicateBorder(image);
		finish_duplicate = MPI_Wtime();
		//linear_algorithm
		start_linear = MPI_Wtime();
		data = copy.data;
		res_linear.data = processImage(data, copy.rows, copy.cols);
		finish_linear = MPI_Wtime();
		
	}
	MPI_Barrier(MPI_COMM_WORLD);
	start_pp = MPI_Wtime();
	if (rank == 0)
		if (size > rows)
		{
			cout << "process must be less than rows" << endl;
			MPI_Finalize();
			return -1;
		}
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	count = new int[size];

	if (rank == 0) {
		displs = new int[size];
		//count = new int[size];
		sent_position_row = 0;
		block = rows / size;
		left = rows % size;
		//count and right position
		for (int i = 0; i < size; i++)
		{
			count[i] = block * (cols + 2) + 2 * (cols + 2);
			if (left > 0)
			{
				count[i] += (cols + 2);
				left--;
			}
			displs[i] = sent_position_row;
			sent_position_row += count[i] - 2 * (cols + 2);
		}
	}
	MPI_Bcast(count, size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&block,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&rows,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
	local_res = new uchar[(count[rank] / (cols + 2) - 2)*cols];
	if (rank > 0) 
	{
		data = new uchar[rows*cols];
	}
	tmp = new uchar	[count[rank]];
	MPI_Scatterv(data, count, displs, MPI_UNSIGNED_CHAR, tmp, count[rank], MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
	cout << "i am here" << endl;
	local_res = processImage(tmp, count[rank] / (cols + 2), cols + 2);

	res_pp.data = data;
	finish_pp = MPI_Wtime();
	if (rank == 0) {
		namedWindow("Image", WINDOW_NORMAL);
		namedWindow("copy", WINDOW_NORMAL);
		namedWindow("res linear", WINDOW_NORMAL);
		namedWindow("res pp", WINDOW_NORMAL);
		imshow("Image", image);
		imshow("copy", copy);
		imshow("res linear", res_linear);
		imshow("res pp", res_pp);
		cout << "dublicate time " << finish_duplicate - start_duplicate << endl;
		cout << "linear time " << finish_linear - start_linear << endl;
		cout << "pp time " << finish_pp - start_pp << endl;
		waitKey();
	}
	delete[] tmp;
	MPI_Finalize();
	return 0;
}