//#include <iostream>
//#include <opencv2/opencv.hpp>
//using namespace std;
//using namespace cv;
//
//Mat duplicateBorder(Mat image)
//{
//	Mat new_image(image.rows + 2, image.cols + 2, CV_8UC1);
//	for (int i = 0; i < new_image.rows; i++)
//		for (int j = 0; j < new_image.rows; j++)
//			new_image.at<uchar>(i, j) = 128;
//	new_image.at<uchar>(0, 0) = image.at<uchar>(0, 0);
//	new_image.at<uchar>(0, new_image.cols - 1) = image.at<uchar>(0, image.cols - 1);
//	new_image.at<uchar>(new_image.rows - 1, 0) = image.at<uchar>(image.rows - 1, 0);
//	new_image.at<uchar>(new_image.rows - 1, new_image.cols - 1) = image.at<uchar>(image.rows - 1, image.cols - 1);
//	for (int i = 0; i < image.cols; i++)
//	{
//		new_image.at<uchar>(0, i + 1) = image.at<uchar>(0, i);
//		new_image.at<uchar>(new_image.rows - 1, i + 1) = image.at<uchar>(image.rows - 1, i);
//	}
//	for (int i = 0; i < image.rows; i++)
//	{
//		new_image.at<uchar>(i + 1, 0) = image.at<uchar>(i, 0);
//		new_image.at<uchar>(i + 1, new_image.cols - 1) = image.at<uchar>(i, image.cols - 1);
//	}
//	for (int i = 0; i < image.rows; i++)
//		for (int j = 0; j < image.cols; j++)
//		{
//			new_image.at<uchar>(i + 1, j + 1) = image.at<uchar>(i, j);
//		}
//
//	return new_image;
//}
//
//int main() 
//{
//	double kernel[] = { 1,2,1,2,4,2,1,2,1 };
//	for (int i = 0; i < 9; i++)
//		kernel[i] /= 16;
//	Mat image = imread("D:\\GitProject\\parallel-programming-1\\1706-4\\dobrohotov_vn\\Lab 1\\Picture\\best.jpg");
//	Mat res(image.rows, image.cols, CV_8UC1);
//	Mat copy = duplicateBorder(image);
//	for (int i = 0; i < 9; i++) 
//	{
//
//	}
//	for (int k=)
//	for (int i=-1;i<=1;i++)
//		for (int j = -1; j <= 1; j++) 
//		{
//		
//		}
//	return 0;
//}
