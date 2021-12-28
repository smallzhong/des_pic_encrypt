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
	init_des("zyc9075 ");

	Mat backImg = imread("g:/lena1.png");  //存放自己图像的路径 
	//imshow("显示图像", backImg);

	//colorReduce(backImg);

	char(*origin_image_buffer)[8] = (char(*)[8])malloc(backImg.rows * backImg.cols * backImg.channels());
	if (backImg.isContinuous())
	{
		memcpy(origin_image_buffer, backImg.data, backImg.rows * backImg.cols * backImg.channels());
	}

	char(*encrypt_image_buffer)[8] = (char(*)[8])malloc(backImg.rows * backImg.cols * backImg.channels());
	memcpy((void*)encrypt_image_buffer, (void*)origin_image_buffer, backImg.rows * backImg.cols * backImg.channels());

	uchar* ttt = (uchar*)encrypt_image_buffer;
	for (int i = 0; i < 196608 / 8; i++)
	{
		/*	bitset<64> t;
			memcpy(&t, (void *)(ttt[i * 8]), 8);*/
		bitset<64> origin;
		memcpy(&origin, (void*)(ttt + i * 8), 8);

		bitset<64> after;
		after = encrypt(origin);
		memcpy((void*)(ttt + (i * 8)), &after, 8);
		//t ^= 123;

		//memcpy((void *)ttt[i * 8], &t, 8);
	}

	memcpy(backImg.data, origin_image_buffer, backImg.rows * backImg.cols * backImg.channels());
	imshow("加密前的图片", backImg);

	memcpy(backImg.data, encrypt_image_buffer, backImg.rows * backImg.cols * backImg.channels());
	imshow("加密后的图片", backImg);
	//imwrite("after_en.png", backImg);

	waitKey(0);
	return 0;
}