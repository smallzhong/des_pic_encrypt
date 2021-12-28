#include <opencv2/opencv.hpp>
#include <iostream>
#include "head.hpp"

using namespace cv;



void colorReduce(Mat& image)
{
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (image.channels() == 1)
			{
				//取出灰度图像中i行j列的点
				image.at<uchar>(i, j) = image.at<uchar>(i, j) / 2;
			}
			else if (image.channels() == 3)
			{
				//取出彩色图像中i行j列第k(0/1/2)通道的颜色点
				image.at<Vec3b>(i, j)[0] = image.at<Vec3b>(i, j)[0] / 2;
				image.at<Vec3b>(i, j)[1] = image.at<Vec3b>(i, j)[1] / 2;
				image.at<Vec3b>(i, j)[2] = image.at<Vec3b>(i, j)[2] / 2;
			}
		}
	}
}

int main()
{
	Mat backImg = imread("g:/lena1.png");  //存放自己图像的路径 
	imshow("显示图像", backImg);

	//colorReduce(backImg);

	char(*p)[8] = (char(*)[8])malloc(backImg.rows * backImg.cols * backImg.channels());
	if (backImg.isContinuous())
	{
		memcpy(p, backImg.data, backImg.rows * backImg.cols * backImg.channels());
	}

	uchar* ttt = (uchar*)p;
	for (int i = 0; i < 196608; i++)
	{
		ttt[i] ^= 123;
	}

	memcpy(backImg.data, p, backImg.rows * backImg.cols * backImg.channels());

	imshow("显示图像_after", backImg);

	waitKey(0);
	return 0;
}