#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <opencv2/opencv.hpp>

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
            std::cout << kernel[i + j + 2 * (i + 2)] <<"  ";
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
    cv::Mat filter = cv::Mat::zeros(image.rows-2, image.cols-2, CV_8UC1);
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

void gaussForThread(const cv::Mat& src, cv::Mat& dst ,const double* kernel, int start, int finish)
{
    for (int x = start + 1; x < finish; ++x)
        for (int y = 1; y < src.rows - 1; ++y)
        {
            double tmp = 0;
            for (int i = -1; i <= 1; ++i)
                for (int j = -1; j <= 1; j++)
                {
                    tmp += src.at<uchar>(y + i, x + j) * kernel[i + j + 2 * (i + 2)];
                }
            dst.at<uchar>(y - 1, x - 1) = Clamp(static_cast<int>(tmp));
        }
}

cv::Mat gaussThread(const cv::Mat& image, const double*const kernel,const int num_threads)
{
    cv::Mat filter = cv::Mat::zeros(image.rows - 2, image.cols - 2, CV_8UC1);
    std::vector<std::thread> thr(num_threads);
    std::vector<int> block(num_threads, filter.cols / num_threads);
    int left = filter.cols % num_threads;
    if (left > 0)
    {
        int i = 0;
        while (left != 0)
        {
            block[i]++;
            left--;
        }
    }
    int step = 0;
    for (int i = 0; i < num_threads; ++i) 
    {
        thr[i] = std::thread(gaussForThread, std::cref(image), std::ref(filter), std::cref(kernel), step, step + block[i]+1);
        step += block[i];  
    }
    for (int i = 0; i < num_threads; ++i)
    {
        thr[i].join();
    }
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
    double* kernel;
	if (argc < 2)
	    path_to_image += "../Image/sm.jpg";
	else
		path_to_image += argv[1];
	original = cv::imread(path_to_image);
	if (original.empty()) {
		std::cout << "Error load image" << std::endl;
		return -1;
	}
	if (original.channels() > 1) {
		cv::cvtColor(original, original, cv::COLOR_BGR2GRAY);
		std::cout << "image to grey color" << std::endl;
	}
    duplicate = duplicateBorder(original);
    kernel = createKernel(0.85);


    //Linear
    auto begin_seq = std::chrono::high_resolution_clock::now();
    cv::Mat filter_seq = gaussSeq(duplicate, kernel);
    auto end_seq = std::chrono::high_resolution_clock::now();

    //std::threads
    auto begin_threads = std::chrono::high_resolution_clock::now();
    cv::Mat filter_thread = gaussThread(duplicate, kernel, 4);
    auto end_threads = std::chrono::high_resolution_clock::now();

    std::cout <<"Check  "<< checkImages(filter_seq, filter_thread) << std::endl;
    std::cout << "Time seq " << std::chrono::duration<float>(end_seq - begin_seq).count() << std::endl;
    std::cout << "Time threads " << std::chrono::duration<float>(end_threads - begin_threads).count() << std::endl;
    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow("Filter", cv::WINDOW_NORMAL);
    cv::imshow("Original", original);
    cv::imshow("Filter", filter_thread);
    cv::waitKey();
	return 0;
}
