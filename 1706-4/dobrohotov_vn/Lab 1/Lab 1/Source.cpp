#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <tbb/tbb.h>
#include <tbb/blocked_range.h>
#include <tbb/tick_count.h>
#include <tbb/parallel_for.h>
//#include <tbb/partitioner.h>

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
                tmp += (static_cast <int>(src->at<uchar>(y + i, number_col + j))) * kernel[1 + i][1 + j];
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

cv::Mat gauss_seq(cv::Mat src,double kernel[3][3])
{
    cv::Mat res(src.rows, src.cols, CV_8UC1);
    cv::Mat duplicate = duplicateBorder(src);
    for (int y = 1; y < duplicate.rows - 1; ++y)
        for (int x = 1; x < duplicate.cols - 1; ++x)
        {
            int tmp = 0;
            for (int i = -1; i <= 1; ++i)
                for (int j = -1; j <= 1; ++j)
                {
                    tmp += ((int)duplicate.at<uchar>(y + i, x + j)) * kernel[1 + i][1 + j];
                }
            res.at<uchar>(y - 1, x - 1) = (uchar)Clamp(tmp);
        }
    return res;
}

bool check_res(cv::Mat image1, cv::Mat image2)
{
    if (image1.rows != image2.rows || image1.cols != image2.cols)
        return false;
    for (int i = 0;i < image1.rows; ++i)
        for (int j = 0; j < image1.cols; ++j)
        {
            if (image1.at<uchar>(i, j) != image2.at<uchar>(i, j))
                return false;
        }
    return true;
}

int main() 
{
    std::string path_to_image="../Image/big_bus.jpg";
   // std::string path_to_image = "big_bus.jpg";
    cv::Mat original, duplicate;
    double kernel[3][3] = { {1,2,1},{ 2,4,2},{1,2,1} };
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            kernel[i][j] /= 16;
    original = cv::imread(path_to_image);
    cv::Mat filter(original.rows, original.cols, CV_8UC1);
    cv::Mat seq;
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
    duplicate = duplicateBorder(original);
    int count_threads = tbb::task_scheduler_init::default_num_threads();
    tbb::task_scheduler_init init(count_threads);
    tbb::tick_count start = tbb::tick_count::now();
    tbb::parallel_for(tbb::blocked_range<size_t>(1, duplicate.cols-1, 5000), [&duplicate, &filter, &kernel](const tbb::blocked_range<size_t>& r)
    {
        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            gauss_for_tbb(&duplicate, &filter, kernel, i);
        }
    });
    tbb::tick_count finish = tbb::tick_count::now();
    tbb::tick_count start_seq = tbb::tick_count::now();
    seq = gauss_seq(original, kernel);
    tbb::tick_count finish_seq = tbb::tick_count::now();
    std::cout << "Threads  " << count_threads << std::endl;
    std::cout << "Seq  " << (finish_seq - start_seq).seconds() << " sec" << std::endl;
    std::cout << "Paralel  " << (finish - start).seconds() << " sec" << std::endl;
    std::cout << "Check " << check_res(filter, seq) << std::endl;
    //cv::namedWindow("Original", cv::WINDOW_NORMAL);
    //cv::namedWindow("Duplicate", cv::WINDOW_NORMAL);
    //cv::namedWindow("Filter", cv::WINDOW_NORMAL);
    //cv::imshow("Original", original);
    //cv::imshow("Duplicate", duplicate);
    //cv::imshow("Filter", filter);
    //cv::waitKey();
    // system("pause");
    return 0;
}
