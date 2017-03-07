// Threshold.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;
using namespace std;

/// ȫ�ֱ������弰��ֵ

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

/// �Զ��庯������
void Threshold_Demo(int, void*);

/**
* @������
*/
int main(int argc, char** argv)
{
	/// ��ȡһ��ͼƬ�����ı�ͼƬ�������ɫ���ͣ��ö�ȡ��ʽΪDOS����ģʽ��
	src = imread(src_path, 1);

	/// ��ͼƬת���ɻҶ�ͼƬ
	cvtColor(src, src_gray, CV_RGB2GRAY);

	/// ����һ��������ʾͼƬ
	namedWindow(window_name, CV_WINDOW_NORMAL);

	/// ������������������ֵ
	createTrackbar(trackbar_type,
		window_name, &threshold_type,
		max_type, Threshold_Demo);

	createTrackbar(trackbar_value,
		window_name, &threshold_value,
		max_value, Threshold_Demo);

	/// ��ʼ���Զ������ֵ����
	Threshold_Demo(0, 0);

	/// �ȴ��û������������ESC�����˳��ȴ����̡�
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
* @�Զ������ֵ����
*/
void Threshold_Demo(int, void*)
{
	/* 0: ��������ֵ
	1: ����������ֵ
	2: �ض���ֵ
	3: 0��ֵ
	4: ��0��ֵ
	*/
	//equalizeHist(src_gray, src_gray);
	if (threshold_type != 5)
		threshold(src_gray, dst, threshold_value, max_BINARY_value, threshold_type);
	else
		//threshold(src_gray, dst, threshold_value, max_BINARY_value, CV_THRESH_BINARY | CV_THRESH_OTSU);
	adaptiveThreshold(src_gray, dst, 70, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 1);
	

	imshow(window_name, dst);
}
