#include <opencv2/opencv.hpp>
#include <iostream>
#include "head.hpp"

using namespace cv;

int buf_size;

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

void encrypt_image(uchar* en_buf)
{
	for (int i = 0; i < buf_size / 8; i++)
	{
		bitset<64> origin;
		memcpy(&origin, (void*)(en_buf + i * 8), 8);

		bitset<64> after;
		after = encrypt(origin);
		memcpy((void*)(en_buf + (i * 8)), &after, 8);
	}
}

void decrypt_image(uchar* de_buf)
{
	for (int i = 0; i < buf_size / 8; i++)
	{
		bitset<64> origin;
		memcpy(&origin, (void *)(de_buf + i * 8), 8);
		bitset<64> after;
		after = decrypt(origin);
		memcpy((void *)(de_buf + (i * 8)), &after, 8);
	}
}

int main()
{
	// 初始化des密钥
	init_des("zyc9075 ");

	// 读取图像
	Mat backImg = imread("g:/lena1.png");
	buf_size = backImg.cols * backImg.rows * backImg.channels();

	// 原始图像buffer
	char(*origin_image_buffer)[8] = (char(*)[8])malloc(backImg.rows * backImg.cols * backImg.channels());
	if (backImg.isContinuous())
	{
		memcpy(origin_image_buffer, backImg.data, backImg.rows * backImg.cols * backImg.channels());
	}

	// 加密图像buffer
	char(*encrypt_image_buffer)[8] = (char(*)[8])malloc(backImg.rows * backImg.cols * backImg.channels());
	memcpy((void*)encrypt_image_buffer, (void*)origin_image_buffer, backImg.rows * backImg.cols * backImg.channels());

	// 解密后的图像的buffer
	char(*decrypt_image_buffer)[8] = (char(*)[8])malloc(backImg.rows * backImg.cols * backImg.channels());

	// 进行图像加密
	encrypt_image((uchar *)encrypt_image_buffer);

	// 进行图像解密
	memcpy((void *)decrypt_image_buffer, (void *)encrypt_image_buffer, buf_size);
	decrypt_image((uchar *)decrypt_image_buffer);

	memcpy(backImg.data, origin_image_buffer, backImg.rows * backImg.cols * backImg.channels());
	imshow("加密前的图片", backImg);

	memcpy(backImg.data, encrypt_image_buffer, backImg.rows * backImg.cols * backImg.channels());
	imshow("加密后的图片", backImg);

	memcpy(backImg.data, decrypt_image_buffer, backImg.rows * backImg.cols * backImg.channels());
	imshow("重新解密后的图片", backImg);

	//imwrite("after_en.png", backImg);

	waitKey(0);
	return 0;
}