// Canny.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

const int msfsp = 20;		//作为meanshift filter的像素位置差值
const int msfsr = 10;		//作为Meanshift filter的像素大小差值
/// 全局变量

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
* @函数 CannyThreshold
* @简介： trackbar 交互回调 - Canny阈值输入比例1:3
*/
void CannyThreshold(int, void*)
{
	/// 使用 3x3内核降噪
	//equalizeHist(src_gray, src_gray);
	Mat src_erod = src_gray.clone();
	Mat kernal(3, 3, CV_8U, cv::Scalar(255));
	
	blur(src_erod, detected_edges, Size(1, 1));

	//erode(src_erod, src_erod, kernal);
	//dilate(src_erod, src_erod, kernal);
	/// 运行Canny算子
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

	/// 使用 Canny算子输出边缘作为掩码显示原图像
	dst = Scalar::all(0);

	src.copyTo(dst, detected_edges);
	//dst = dst + src;
	imshow(window_name, dst);
}


/** @函数 main */
int main(int argc, char** argv)
{
	/// 装载图像
	src = imread(src_path);

	if (!src.data)
	{
		return -1;
	}

	/// 创建与src同类型和大小的矩阵(dst)
	dst.create(src.size(), src.type());

	/// 原图像转换为灰度图像
	pyrMeanShiftFiltering(src, src, msfsp, msfsr);		//进行meanshift滤波
	cvtColor(src, src_gray, CV_BGR2GRAY);

	/// 创建显示窗口
	namedWindow(window_name, CV_WINDOW_NORMAL);

	/// 创建trackbar
	createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

	/// 显示图像
	CannyThreshold(0, 0);

	/// 等待用户反应
	waitKey(0);

	return 0;
}
