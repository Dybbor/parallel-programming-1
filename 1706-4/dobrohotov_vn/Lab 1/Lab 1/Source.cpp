#include <iostream>
#include <string>
#include <chrono>
#include <omp.h>

#include <opencv2/opencv.hpp>

#define M_PI 3.14159265358979323846

double* Kernel()
{
    double kernel[9] = { 1,2,1,2,4,2,1,2,1 };
    //normirovvka intensity
    for (int i = 0; i < 9; ++i)
        kernel[i] /= 16;
    return  kernel;
}
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
void PrintData(cv::Mat image)
{
    std::cout << std::endl;
    for (int i = 0; i < image.rows*image.cols; ++i)
    {
        if (i % image.cols == 0 && i != 0)
            std::cout << std::endl;
        std::cout << static_cast<int>(image.data[i]) << "  ";
    }
}
void PrintMat(cv::Mat image)
{
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            std::cout << (int)image.at<uchar>(i, j) << "  ";
        }
        std::cout << std::endl;
    }
}

int Clamp(int num)
{
    if (num > 255)
        return 255;
    else if (num < 0)
        return 0;
    return num;

}
cv::Mat duplicateBorder(cv::Mat image)
{
    cv::Mat new_image(image.rows + 2, image.cols + 2, CV_8UC1);
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

int main(int argc, char** argv)
{

    std::string path_to_image;
    cv::Mat original, duplicate;
    double kernel[3][3] = { {1,2,1},{ 2,4,2},{1,2,1} };
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            kernel[i][j] /= 16;
    std::cout << argc << " " << argv[0] << " " << std::endl;
    if (argc < 2)
        path_to_image += "../Image/big.jpg";
    else
        path_to_image += argv[1];
    original = cv::imread(path_to_image);
    if (!original.data)
    {
        std::cout << "Error load image" << std::endl;
        return -1;
    }
    if (original.channels() > 1)
    {
        cv::cvtColor(original, original, cv::COLOR_BGR2GRAY);
        std::cout << "image to grey color" << std::endl;
    }
    cv::Mat  filter(original.rows, original.cols, CV_8UC1);
    for (int i = 0; i < filter.rows; ++i)
        for (int j = 0; j < filter.rows; ++j)
            filter.at<uchar>(i, j) = 0;
    namedWindow("Original", cv::WINDOW_NORMAL);
    namedWindow("Duplicate", cv::WINDOW_NORMAL);
    namedWindow("Filter", cv::WINDOW_NORMAL);
    duplicate = duplicateBorder(original);
    //Linear alghorithm

    auto begin = std::chrono::steady_clock::now();
#pragma omp parallel
    {
    #pragma omp for
        for (int x = 1; x < duplicate.cols - 1; ++x)
            for (int y = 1; y < duplicate.rows - 1; ++y)
            {
                int tmp = 0;
                for (int i = -1; i <= 1; ++i)
                    for (int j = -1; j <= 1; ++j)
                    {
                        tmp += (static_cast <int>(duplicate.at<uchar>(y + i, x + i))) * kernel[1 + i][1 + j];
                    }
                filter.at<uchar>(y - 1, x - 1) = (uchar)Clamp(tmp);
            }
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "The time: " << elapsed_ms.count() << " ms\n";
    cv::imshow("Original", original);
    cv::imshow("Duplicate", duplicate);
    cv::imshow("Filter", filter);
    cv::waitKey();
    return 0;
}