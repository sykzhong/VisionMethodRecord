// Threshold.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;
using namespace std;

/// 全局变量定义及赋值

int threshold_value = 0;
int threshold_type = 3;
int const max_value = 255;
int const max_type = 5;
int const max_BINARY_value = 255;

//string src_path = "E:/Cloud/Research/Vision/Picture/workpiece_v1/workpiece01_template.jpg";
//string src_path = "E:/Cloud/Research/Vision/Picture/workpiece_v2/workpiece_2(shrinked).jpg";
string src_path = "E:/Cloud/Research/Vision/MeanshiftTest/MeanshiftTest/meanshiftdst3.jpg";

Mat src, src_gray, dst;
char* window_name = "Threshold Demo";

char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Value";

/// 自定义函数声明
void Threshold_Demo(int, void*);

/**
* @主函数
*/
int main(int argc, char** argv)
{
	/// 读取一副图片，不改变图片本身的颜色类型（该读取方式为DOS运行模式）
	src = imread(src_path, 1);

	/// 将图片转换成灰度图片
	cvtColor(src, src_gray, CV_RGB2GRAY);

	/// 创建一个窗口显示图片
	namedWindow(window_name, CV_WINDOW_NORMAL);

	/// 创建滑动条来控制阈值
	createTrackbar(trackbar_type,
		window_name, &threshold_type,
		max_type, Threshold_Demo);

	createTrackbar(trackbar_value,
		window_name, &threshold_value,
		max_value, Threshold_Demo);

	/// 初始化自定义的阈值函数
	Threshold_Demo(0, 0);

	/// 等待用户按键。如果是ESC健则退出等待过程。
	while (true)
	{
		int c;
		c = waitKey(20);
		if ((char)c == 27)
		{
			break;
		}
	}

}


/**
* @自定义的阈值函数
*/
void Threshold_Demo(int, void*)
{
	/* 0: 二进制阈值
	1: 反二进制阈值
	2: 截断阈值
	3: 0阈值
	4: 反0阈值
	*/
	//equalizeHist(src_gray, src_gray);
	if (threshold_type != 5)
		threshold(src_gray, dst, threshold_value, max_BINARY_value, threshold_type);
	else
		//threshold(src_gray, dst, threshold_value, max_BINARY_value, CV_THRESH_BINARY | CV_THRESH_OTSU);
	adaptiveThreshold(src_gray, dst, 70, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 1);
	

	imshow(window_name, dst);
}
