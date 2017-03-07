// Canny.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

const int msfsp = 20;		//��Ϊmeanshift filter������λ�ò�ֵ
const int msfsr = 10;		//��ΪMeanshift filter�����ش�С��ֵ
/// ȫ�ֱ���

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 127;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";
//std::string src_path = "E:/Cloud/Research/Vision/Picture/workpiece_v2/workpiece_2(shrinked).jpg";
std::string src_path = "iterwp_000.bmp";

/**
* @���� CannyThreshold
* @��飺 trackbar �����ص� - Canny��ֵ�������1:3
*/
void CannyThreshold(int, void*)
{
	/// ʹ�� 3x3�ں˽���
	//equalizeHist(src_gray, src_gray);
	Mat src_erod = src_gray.clone();
	Mat kernal(3, 3, CV_8U, cv::Scalar(255));
	
	blur(src_erod, detected_edges, Size(1, 1));

	//erode(src_erod, src_erod, kernal);
	//dilate(src_erod, src_erod, kernal);
	/// ����Canny����
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

	/// ʹ�� Canny���������Ե��Ϊ������ʾԭͼ��
	dst = Scalar::all(0);

	src.copyTo(dst, detected_edges);
	//dst = dst + src;
	imshow(window_name, dst);
}


/** @���� main */
int main(int argc, char** argv)
{
	/// װ��ͼ��
	src = imread(src_path);

	if (!src.data)
	{
		return -1;
	}

	/// ������srcͬ���ͺʹ�С�ľ���(dst)
	dst.create(src.size(), src.type());

	/// ԭͼ��ת��Ϊ�Ҷ�ͼ��
	pyrMeanShiftFiltering(src, src, msfsp, msfsr);		//����meanshift�˲�
	cvtColor(src, src_gray, CV_BGR2GRAY);

	/// ������ʾ����
	namedWindow(window_name, CV_WINDOW_NORMAL);

	/// ����trackbar
	createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

	/// ��ʾͼ��
	CannyThreshold(0, 0);

	/// �ȴ��û���Ӧ
	waitKey(0);

	return 0;
}
