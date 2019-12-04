#include <iostream>
#include <opencv2/opencv.hpp>
#include "mpi.h"

int main(int argc,char**argv) 
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	/*if (size == 1)
		return -1;*/
	if (rank == 0) 
	{
		cv::Mat image = cv::imread("C:\\Users\\1\\Pictures\\awd.jpg");
		if (!image.data)
			std::cout << "Error load image" << std::endl;
		cv::imshow("Image", image);
		cv::waitKey();
	}
	MPI_Finalize();
	return 0;
}