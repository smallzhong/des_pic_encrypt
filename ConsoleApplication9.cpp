#include <opencv2/opencv.hpp>
#include <iostream>
#include "head.hpp"
//#include "getopt.h"

using namespace cv;

extern "C" char* optarg;

extern "C" int optind;

extern "C" int opterr;

extern "C" int optopt;

int buf_size;
char g_e_or_d;
int g_encrypt_type;
char g_input_file[0x1000];
char g_output_file[0x1000];

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

void encrypt_image_ECB(uchar* en_buf)
{
	for (int i = 0; i < buf_size / 8; i++)
	{
		bitset<64> origin;
		memcpy(&origin, (void*)(en_buf + i * 8), 8);

		bitset<64> after;
		after = encrypt_ECB(origin);
		memcpy((void*)(en_buf + (i * 8)), &after, 8);
	}
}

void decrypt_image_ECB(uchar* de_buf)
{
	for (int i = 0; i < buf_size / 8; i++)
	{
		bitset<64> origin;
		memcpy(&origin, (void*)(de_buf + i * 8), 8);
		bitset<64> after;
		after = decrypt_ECB(origin);
		memcpy((void*)(de_buf + (i * 8)), &after, 8);
	}
}

void encrypt_image_CBC(uchar* en_buf, bitset<64> iv)
{
	for (int i = 0; i < buf_size / 8; i++)
	{
		bitset<64> origin;
		memcpy(&origin, (void*)(en_buf + i * 8), 8);

		bitset<64> after;
		after = encrypt_CBC(origin, iv);

		memcpy(&iv, &after, 8); // 更新iv
		memcpy((void*)(en_buf + (i * 8)), &after, 8);
	}
}

void decrypt_image_CBC(uchar* de_buf, bitset<64> iv)
{
	for (int i = 0; i < buf_size / 8; i++)
	{
		bitset<64> origin;
		memcpy(&origin, (void*)(de_buf + i * 8), 8);
		bitset<64> after;
		after = decrypt_CBC(origin, iv);
		//char t[123];
		//memset(t, 0, sizeof(t));
		//memcpy(t, &iv, sizeof(long long));
		//printf("%s\n", t);

		memcpy(&iv, &origin, 8); // 更新iv
		memcpy((void*)(de_buf + (i * 8)), &after, 8);
	}
}

void encrypt_image_EDE2_CBC(uchar* en_buf, bitset<64> key1, bitset<64> key2, bitset<64> iv)
{
	// 初始化第一个key
	init_des_bitset(key1);

	// 第一步：key1加密
	encrypt_image_CBC(en_buf, iv);

	// 初始化第二个key
	init_des_bitset(key2);

	// 第二步：key2解密
	decrypt_image_CBC(en_buf, iv);

	// 初始化第一个key
	init_des_bitset(key1);

	// 第三步：key1加密第二次
	encrypt_image_CBC(en_buf, iv);
}

void decrypt_image_EDE2_CBC(uchar* de_buf, bitset<64> key1, bitset<64> key2, bitset<64> iv)
{
	// 初始化第一个key
	init_des_bitset(key1);

	// 第一步：key1解密
	decrypt_image_CBC(de_buf, iv);

	// 初始化第二个key
	init_des_bitset(key2);

	// 第二步：key2加密
	encrypt_image_CBC(de_buf, iv);

	// 初始化第一个key
	init_des_bitset(key1);

	// 第三步：key1解密第二次
	decrypt_image_CBC(de_buf, iv);
}

void print_and_die()
{
	printf("  [ Yuchu Pic Encrypter v0.0.1\n"
		"  [ github.com/smallzhong\n"
		"\t-t <type> encrypt type you want to use, 1:ECB 2:CBC 3:EDE2_CBC(default)\n"
		"\t-e Encrypt the input image\n"
		"\t-d Decrypt the input image\n"
		"\t-i <filepath> input image path\n"
		"\t-o <filepath> output image path\n");

	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		print_and_die();
		return 0;
	}

	char ch;
	while ((ch = getopt(argc, argv, "edt:i:o:")) != -1)
	{
		switch (ch)
		{
		case 'e':
		{
			if (g_e_or_d != '\0')
				print_and_die();
			g_e_or_d = 'e';
			break;
		}
		case 'd':
		{
			if (g_e_or_d != '\0')
				print_and_die();
			g_e_or_d = 'd';
			break;
		}
		case 'i':
		{
			printf("输入文件为：%s\n", optarg);
			strcpy(g_input_file, optarg);
			break;
		}
		case 'o':
		{
			printf("输出文件为：%s\n", optarg);
			strcpy(g_output_file, optarg);
			break;
		}
		case 't':
		{
			if (g_encrypt_type != 0) 
				print_and_die();
			g_encrypt_type = optarg[0] - '0';
			break;
		}
		default:
			print_and_die();
			break;
		}
	}

	if (g_encrypt_type == 0)
	{
		// 默认为3
		g_encrypt_type = 3;
	}

	if (*g_input_file == '\0')
	{
		printf("请指定输入文件！\n");
		exit(EXIT_FAILURE);
	}

	if (*g_output_file == '\0')
	{
		printf("请指定输出文件！\n");
		exit(EXIT_FAILURE);
	}


	clock_t a1 = clock();

	// 初始化des密钥
	init_des("zyc9075 ");

	// 读取图像
	Mat backImg = imread(g_input_file);

	if (!backImg.data)
	{
		EXIT_ERROR("读取图像失败！");
	}

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
	//encrypt_image_ECB((uchar*)encrypt_image_buffer);
	//encrypt_image_CBC((uchar*)encrypt_image_buffer, charToBitset("80031190"));
	encrypt_image_EDE2_CBC((uchar*)encrypt_image_buffer, charToBitset("12345678"),
		charToBitset("87654321"), charToBitset("8003119075"));

	// 进行图像解密
	memcpy((void*)decrypt_image_buffer, (void*)encrypt_image_buffer, buf_size);
	//decrypt_image_ECB((uchar*)decrypt_image_buffer);
	//decrypt_image_CBC((uchar*)decrypt_image_buffer, charToBitset("80031190"));
	decrypt_image_EDE2_CBC((uchar*)decrypt_image_buffer, charToBitset("12345678"),
		charToBitset("87654321"), charToBitset("8003119075"));

	memcpy(backImg.data, origin_image_buffer, backImg.rows * backImg.cols * backImg.channels());
	imshow("加密前的图片", backImg);
	imwrite("1.png", backImg);

	memcpy(backImg.data, encrypt_image_buffer, backImg.rows * backImg.cols * backImg.channels());
	imshow("加密后的图片", backImg);
	imwrite("2.png", backImg);

	memcpy(backImg.data, decrypt_image_buffer, backImg.rows * backImg.cols * backImg.channels());
	imshow("重新解密后的图片", backImg);

	imwrite("3.png", backImg);

	clock_t a2 = clock();

	double t = (double)(a2 - a1) / CLOCKS_PER_SEC;
	cout << t;

	waitKey(0);
	return 0;
}