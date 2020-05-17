#include <iostream>
#include <string>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>
#include <tbb/tick_count.h>
#include <tbb/parallel_for.h>

#define PI 3.14159265358979323846

double* createKernel(const double sigma)
{
    double *kernel = new double[9];
    double sum = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            kernel[i + j + 2 * (i + 2)] = (exp(-(i * i + j * j) / (2 * sigma * sigma))) / (2 * PI*sigma*sigma);
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

void printKernel(const double* const kernel)
{
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
            std::cout << kernel[i + j + 2 * (i + 2)] << "  ";
        std::cout << std::endl;
    }
}

int Clamp(const int num)
{
    if (num > 255)
        return 255;
    else if (num < 0)
        return 0;
    return num;

}

cv::Mat duplicateBorder(const cv::Mat& image)
{
    cv::Mat new_image(image.rows + 2, image.cols + 2, CV_8UC1);
    new_image.at<uchar>(0, 0) = image.at<uchar>(0, 0);
    new_image.at<uchar>(0, new_image.cols - 1) = image.at<uchar>(0, image.cols - 1);
    new_image.at<uchar>(new_image.rows - 1, 0) = image.at<uchar>(image.rows - 1, 0);
    new_image.at<uchar>(new_image.rows - 1, new_image.cols - 1) = image.at<uchar>(image.rows - 1, image.cols - 1);
    for (int i = 0; i < image.cols; i++) {
        new_image.at<uchar>(0, i + 1) = image.at<uchar>(0, i);
        new_image.at<uchar>(new_image.rows - 1, i + 1) = image.at<uchar>(image.rows - 1, i);
    }
    for (int i = 0; i < image.rows; i++) {
        new_image.at<uchar>(i + 1, 0) = image.at<uchar>(i, 0);
        new_image.at<uchar>(i + 1, new_image.cols - 1) = image.at<uchar>(i, image.cols - 1);
    }
    image.copyTo(new_image(cv::Rect(1, 1, image.cols, image.rows)));
    return new_image;
}

cv::Mat gaussSeq(const cv::Mat& image, const double*const kernel)
{
    cv::Mat filter = cv::Mat::zeros(image.rows - 2, image.cols - 2, CV_8UC1);
    for (int y = 1; y < image.rows - 1; ++y)
        for (int x = 1; x < image.cols - 1; ++x)
        {
            double tmp = 0;
            for (int i = -1; i <= 1; ++i)
                for (int j = -1; j <= 1; j++)
                {
                    tmp += image.at<uchar>(y + i, x + j) * kernel[i + j + 2 * (i + 2)];
                }
            filter.at<uchar>(y - 1, x - 1) = Clamp(static_cast<int>(tmp));
        }
    return filter;
}

cv::Mat gaussOpenMP(const cv::Mat& image, const double*const kernel, int count_threads)
{
    cv::Mat filter = cv::Mat::zeros(image.rows - 2, image.cols - 2, CV_8UC1);
    omp_set_num_threads(count_threads);
#pragma omp parallel 
    {
#pragma omp for schedule(dynamic)  
        for (int x = 1; x < image.cols - 1; ++x)
        {
            for (int y = 1; y < image.rows - 1; ++y)
            {
                double tmp = 0;
                for (int i = -1; i <= 1; ++i)
                    for (int j = -1; j <= 1; j++)
                    {
                        tmp += image.at<uchar>(y + i, x + j) * kernel[i + j + 2 * (i + 2)];
                    }
                filter.at<uchar>(y - 1, x - 1) = Clamp(static_cast<int>(tmp));
            }
        }
    }
    return filter;
}

void funcForTBB(const cv::Mat& src, cv::Mat& dst,const double * const kernel, size_t number_col) 
{
    for (int y = 1; y < src.rows - 1; ++y)
    {
        double tmp = 0;
        for (int i = -1; i <= 1; ++i)
            for (int j = -1; j <= 1; ++j)
            {
                tmp += src.at<uchar>(y + i, number_col + j) * kernel[i + j + 2 * (i + 2)];
            }
        dst.at<uchar>(y - 1, number_col - 1) = Clamp(static_cast<int>(tmp));
    }        
}

cv::Mat gaussTBB(const cv::Mat& image, const double* kernel, const int count_threads) 
{
    tbb::task_scheduler_init init(count_threads);
    cv::Mat filter = cv::Mat::zeros(image.rows - 2, image.cols - 2, CV_8UC1);
    tbb::parallel_for(tbb::blocked_range<size_t>(1, image.cols-1), [&image, &filter, &kernel](const tbb::blocked_range<size_t>& r)
    {
        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            funcForTBB(image, filter, kernel, i);
        }
    });
    return filter;
}


bool checkImages(const cv::Mat& image1, const cv::Mat& image2)
{
    cv::Mat res;
    cv::bitwise_xor(image1, image2, res); //2 equal pixel = 0, 2 different pixel = pixel
    if (cv::countNonZero(res) > 0)
        return false;
    else
        return true;
}

int main(int argc, char** argv)
{
    std::string path_to_image;
    cv::Mat original, duplicate;
    int count_threads;
    double* kernel;
    if (argc < 2)
        path_to_image += "../Image/big.jpg";
    else
        path_to_image += argv[1];
    original = cv::imread(path_to_image);
    if (original.empty()) {
        std::cout << "Error load image" << std::endl;
        return -1;
    }
    if (argc < 3)
    {
        count_threads = tbb::task_scheduler_init::default_num_threads();
    }
    else
    {
        count_threads = atoi(argv[2]);
        if (count_threads == 0)
        {
            std::cout << "Error input num threads" << std::endl;
            return -2;
        }
    }
    if (original.channels() > 1) {
        cv::cvtColor(original, original, cv::COLOR_BGR2GRAY);
        std::cout << "image to grey color" << std::endl;
    }
    duplicate = duplicateBorder(original);
    kernel = createKernel(0.85);

    //Linear algorithm
    cv::Mat filter_seq;
    tbb::tick_count start_seq = tbb::tick_count::now();
    filter_seq = gaussSeq(duplicate, kernel);
    tbb::tick_count finish_seq = tbb::tick_count::now();

    //OpenMP
    cv::Mat filter_openMP;
    tbb::tick_count start_openMP = tbb::tick_count::now();
    filter_openMP = gaussOpenMP(duplicate, kernel, count_threads);
    tbb::tick_count finish_openMP = tbb::tick_count::now();

    //TBB
    cv::Mat filter_TBB;
    tbb::tick_count start_TBB = tbb::tick_count::now();
    filter_TBB = gaussTBB(duplicate, kernel, count_threads);
    tbb::tick_count  finish_TBB = tbb::tick_count::now();

    std::cout << "Threads: " << count_threads << std::endl;
    std::cout << "Check algorithms openMP: " << checkImages(filter_seq, filter_openMP) << std::endl;
    std::cout << "Check algorithms TBB: " << checkImages(filter_seq, filter_TBB) << std::endl;
    std::cout << "Time seq:  " << (finish_seq - start_seq).seconds() << std::endl;
    std::cout << "Time openMP:  " << (finish_openMP - start_openMP).seconds() << std::endl;
    std::cout << "Time TBB:  " << (finish_TBB - start_TBB).seconds() << std::endl;
    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow("Filter", cv::WINDOW_NORMAL);
    cv::imshow("Original", original);
    cv::imshow("Filter", filter_openMP);
    cv::waitKey();
    return 0;
}
