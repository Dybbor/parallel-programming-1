//#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <mpi.h>
//using namespace cv;
//using namespace std;
//#define M_PI 3.14159265358979323846
//Mat filterGauss(Mat image)
//{
//	return image;
//}
//
//double* createKernel(double* kernel, double sigma)
//{
//	double sum = 0;
//	for (int i = -1; i <= 1; i++)
//	{
//		for (int j = -1; j <= 1; j++)
//		{
//			kernel[i + j + 2 * (i + 2)] = (double)(exp(-(i*i + j * j) / (sigma*sigma))) *(1/ (sigma*sqrt(2 * M_PI)));
//			sum += kernel[i + j + 2 * (i + 2)];
//		}
//	}
//	for (int i = -1; i <= 1; i++) {
//		for (int j = -1; j <= 1; j++) {
//			kernel[i + j + 2 * (i + 2)] /= sum;
//		}
//	}
//	return kernel;
//}
//
//Mat duplicateBorder(Mat image) 
//{
//
//	Mat new_image(image.rows + 2, image.cols + 2, CV_8UC1);
//	for (int i = 0; i < new_image.rows; i++)
//		for (int j = 0; j < new_image.cols; j++)
//			new_image.at<uchar>(i, j) = 128;
//	new_image.at<uchar>(0, 0) = image.at<uchar>(0, 0);
//	new_image.at<uchar>(0, new_image.cols-1) = image.at<uchar>(0, image.cols-1);
//	new_image.at<uchar>(new_image.rows-1, 0) = image.at<uchar>(image.rows-1, 0);
//	new_image.at<uchar>(new_image.rows-1,new_image.cols-1) = image.at<uchar>(image.rows-1, image.cols-1);
//	for (int i = 0; i < image.cols; i++)
//	{
//		new_image.at<uchar>(0, i + 1) = image.at<uchar>(0, i);
//		new_image.at<uchar>(new_image.rows - 1, i + 1) = image.at<uchar>(image.rows - 1, i);
//	}
//	for (int i = 0; i < image.rows; i++)
//	{
//		new_image.at<uchar>(i+1, 0) = image.at<uchar>(i, 0);
//		new_image.at<uchar>(i+1, new_image.cols-1) = image.at<uchar>(i,image.cols-1);
//	}
//	for (int i=0;i<image.rows;i++)
//		for (int j = 0; j < image.cols; j++) 
//		{
//			new_image.at<uchar>(i + 1, j + 1) = image.at<uchar>(i, j);
//		}
//
//	return new_image;
//}
////uchar*  processImage(uchar* data,double* kernel, int rows, int cols) 
////{
////	uchar *res_data = new uchar[(rows-2)*(cols-2)];
////	int count_res_data = 0;
////	int move = 0;
////	cout << (rows)*(cols) - 2 * (cols) - 1 << " ind" << endl;
////	for (int i = 0; i < (rows)*(cols) - 2 * (cols) - 1; i++)
////	{
////		int res = 0;
////		move = 0;
////		//cout << i;
////		int count_kernel = 0;
////		for (int j = 0; j < 3; j++)
////		{
////			for (int k = 0; k < 3; k++)
////			{
////				//cout <<"   "<<(int)new_image.data[i + j + k + move] << " ";
////				res += data[i + j + k + move] * kernel[count_kernel];
////				count_kernel++;
////			}
////			move += (cols) - 1;
////		}
////		if (res > 255)
////			res = 255;
////		if (res < 0)
////			res = 0;
////		res_data[count_res_data] = res;
////		count_res_data++;
////		if ((i + 3) % (cols) == 0)
////			i += 2;
////	}
////	return res_data;
////}
//
//uchar*  processImage(uchar* data, int rows, int cols)
//{
//	cout << "I am in func" << endl;
//	uchar *res_data = new uchar[(rows - 2)*(cols - 2)];
//	double kernel[9] = { 1,2,1,2,4,2,1,2,1 };
//	//normirovvka intensity
//	for (int i = 0; i < 9; ++i)
//		kernel[i] /= 16;
//	int count_res_data = 0;
//	int move = 0;
//	//cout << (rows)*(cols)-2 * (cols)-1 << " ind" << endl;
//	for (int i = 0; i < ((rows)*(cols)-2 * (cols)-1); i++)
//	{
//		int res = 0;
//		move = 0;
//		//cout << i;
//		int count_kernel = 0;
//		for (int j = 0; j < 3; j++)
//		{
//			for (int k = 0; k < 3; k++)
//			{
//				//cout <<"   "<<(int)new_image.data[i + j + k + move] << " ";
//				res += data[i + j + k + move] * kernel[count_kernel];
//				count_kernel++;
//			}
//			move += (cols)-1;
//		}
//		if (res > 255)
//			res = 255;
//		if (res < 0)
//			res = 0;
//		res_data[count_res_data] = res;
//		count_res_data++;
//		//cout << "asd" << " " << i << endl; 
//		if ((i + 3) % (cols) == 0)
//			i += 2;
//	}
//	return res_data;
//}
//int main() 
//{
//	Mat image = imread("D:\\GitProject\\parallel-programming-1\\1706-4\\dobrohotov_vn\\Lab 1\\Picture\\red.jpg");
//	if (!image.data) {
//		cout << "Error load image" << endl;
//		system("pause");
//		return -1;
//	}
//	double kernel[9] = { 1,2,1,2,4,2,1,2,1 };
//	for (int i = 0; i < 9; ++i)
//		kernel[i] /= 16;
//	/*double *kernel = new double[9];
//	kernel= createKernel(kernel, 10);
//	for (int i = 0; i < 9; i++)
//		cout << kernel[i]<<"  ";*/
//	cvtColor(image, image, COLOR_BGR2GRAY);
////	threshold(image, image, 100, 255, THRESH_BINARY);
//	namedWindow("image", WINDOW_NORMAL);
//	namedWindow("Copy", WINDOW_NORMAL);
//	imshow("image", image);
//	Mat new_image=duplicateBorder(image);
//	/*for (int i = 0; i < new_image.rows; i++)
//	{
//		for (int j = 0; j < new_image.cols; j++) 
//		{
//			cout <<(int) new_image.at<uchar>(i, j) << " ";
//		}
//		cout << endl;
//	}*/
//	uchar *data = new_image.data;
//	//uchar *res_data = new uchar[image.rows*image.cols];
//	//int count_res_data=0;
//	//int move = 0;
//	//cout << new_image.rows*new_image.cols - 2*new_image.cols - 1 << " ind" << endl;
//	//for (int i = 0; i < new_image.rows*new_image.cols-2*new_image.cols-1; i++) 
//	//{
//	//	int res = 0;
//	//	move = 0;
//	//	//cout << i;
//	//	int count_kernel=0;
//	//	for (int j = 0; j < 3; j++) 
//	//	{
//	//		for (int k = 0; k < 3; k++)
//	//		{
//	//			//cout <<"   "<<(int)new_image.data[i + j + k + move] << " ";
//	//			res += new_image.data[i + j + k + move] * kernel[count_kernel];
//	//			count_kernel++;
//	//		}
//	//		move += new_image.cols-1;
//	//	}
//	//	if (res > 255)
//	//		res = 255;
//	//	if (res < 0)
//	//		res = 0;
//	//	res_data[count_res_data] = res;
//	//	count_res_data++;
//	//	if ((i + 3) % new_image.cols == 0)
//	//		i += 2;
//	//}
//	//Mat res(image.rows, image.cols, CV_8UC1);
//	Mat res(image.rows, image.cols, CV_8UC1);
//	res.data = processImage(new_image.data,new_image.rows,new_image.cols);
//	namedWindow("res", WINDOW_NORMAL);
//	imshow("Copy", new_image);
//	imshow("res", res);
//	waitKey(0);
//	return 0;
//}  
//////9232256