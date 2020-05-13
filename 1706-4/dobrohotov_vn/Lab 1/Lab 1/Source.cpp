#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <tbb/tbb.h>
#include <tbb/blocked_range.h>
#include <tbb/tick_count.h>

#define M_PI 3.14159265358979323846

int Clamp(int num)
{
    if (num > 255)
        return 255;
    else if (num < 0)
        return 0;
    return num;
}

void gauss_for_tbb(cv::Mat *src, cv::Mat *dst, double kernel[3][3], int number_col) 
{
    for (int y = 1; y < src->rows - 1; ++y)
    {
        int tmp = 0;
        for (int i = -1; i <= 1; ++i)
            for (int j = -1; j <= 1; ++j)
            {
                tmp += (static_cast <int>(src->at<uchar>(y + i, number_col + i))) * kernel[1 + i][1 + j];
            }
        dst->at<uchar>(y - 1, number_col - 1) = (uchar)Clamp(tmp);
    }
        
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

int main() 
{
    std::string path_to_image="../Image/big.jpg";
    cv::Mat original, duplicate;
    double kernel[3][3] = { {1,2,1},{ 2,4,2},{1,2,1} };
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            kernel[i][j] /= 16;
    original = cv::imread(path_to_image);
    cv::Mat filter(original.rows, original.cols, CV_8UC1);
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
    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow("Duplicate", cv::WINDOW_NORMAL);
    cv::namedWindow("Filter", cv::WINDOW_NORMAL);
    duplicate = duplicateBorder(original);
    tbb::tick_count start = tbb::tick_count::now();
    tbb::task_scheduler_init init();
    tbb::parallel_for(tbb::blocked_range<size_t>(1, duplicate.cols-1), [&duplicate, &filter, &kernel](const tbb::blocked_range<size_t>& r)
    {
        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            gauss_for_tbb(&duplicate, &filter, kernel,i);
        }
    });
    tbb::tick_count finish = tbb::tick_count::now();
    std::cout << (finish - start).seconds() << "sec" << std::endl;
    cv::imshow("Original", original);
    cv::imshow("Duplicate", duplicate);
    cv::imshow("Filter", filter);
    cv::waitKey();
    return 0;
}
