#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

Mat filterGauss(Mat image)
{
	return image;
}
Mat duplicateBorder(Mat image) 
{

	Mat new_image(image.rows + 2, image.cols + 2, CV_8UC1);
	for (int i = 0; i < new_image.rows; i++)
		for (int j = 0; j < new_image.rows; j++)
			new_image.at<uchar>(i, j) = 128;
	new_image.at<uchar>(0, 0) = image.at<uchar>(0, 0);
	new_image.at<uchar>(0, new_image.cols-1) = image.at<uchar>(0, image.cols-1);
	new_image.at<uchar>(new_image.rows-1, 0) = image.at<uchar>(image.rows-1, 0);
	new_image.at<uchar>(new_image.rows-1,new_image.cols-1) = image.at<uchar>(image.rows-1, image.cols-1);
	for (int i = 0; i < image.cols; i++)
	{
		new_image.at<uchar>(0, i + 1) = image.at<uchar>(0, i);
		new_image.at<uchar>(new_image.rows - 1, i + 1) = image.at<uchar>(image.rows - 1, i);
	}
	for (int i = 0; i < image.rows; i++)
	{
		new_image.at<uchar>(i+1, 0) = image.at<uchar>(i, 0);
		new_image.at<uchar>(i+1, new_image.cols-1) = image.at<uchar>(i,image.cols-1);
	}
	for (int i=0;i<image.rows;i++)
		for (int j = 0; j < image.cols; j++) 
		{
			new_image.at<uchar>(i + 1, j + 1) = image.at<uchar>(i, j);
		}

	return new_image;
}
int main() 
{
	Mat image = imread("D:\\GitProject\\parallel-programming-1\\1706-4\\dobrohotov_vn\\Lab 1\\Picture\\red.jpg");
	if (!image.data) {
		cout << "Error load image" << endl;
		system("pause");
		return -1;
	}
	int i = 0;
	cvtColor(image, image, COLOR_BGR2GRAY);

	namedWindow("image", WINDOW_NORMAL);
	namedWindow("Copy", WINDOW_NORMAL);
	cout << image.step << endl;
	int kernel[] = { 1,2,1,2,4,2,1,2,1 };
	imshow("image", image);
	Mat new_image=duplicateBorder(image);
	uchar *data = new_image.data;
	uchar *res_data = new uchar[image.rows*image.cols];
	int count_res_data=0;
	int move = 0;
	for (int i = 0; i < new_image.cols*new_image.rows-move; i++) 
	{
		int res = 0;
		move = 0;
		int count_kernel=0;
		for (int j = 0; j < 3; j++) 
		{
			for (int k = 0; k < 3; k++)
			{
				res += new_image.data[(uint64)i + j + k + move] * kernel[count_kernel];
				count_kernel++;
			}
			move += new_image.cols;
		}
		if (res > 255)
			res = 255;
		//res_data[count_res_data] = res;
		count_res_data++;
		//if ((i+new_image.cols-3+1)%new_image.cols)
		if ((i + 1) % image.cols == 0)
			i += 2;
	}
	imshow("Copy", new_image);
	waitKey(0);
	return 0;
}  
//9232256