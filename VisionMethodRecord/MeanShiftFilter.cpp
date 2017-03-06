// MeanshiftTest.cpp : 定义控制台应用程序的入口点。
//Meanshigt滤波器

#include "stdafx.h"
#include <iostream>  
#include <vector>  
#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

//string strSrc = "E:/Cloud/Research/Vision/Picture/gzt_v3/gzt01_shrinked_2.jpg";   //整体图像地址名
//string strSrc = "E:/Cloud/Research/Vision/Picture/workpiece_v1/workpiece01.jpg";
//string strSrc = "E:/Cloud/Research/Vision/Picture/workpiece_v2/template_2(shrinked).jpg";
string strSrc = "E:/Cloud/Research/Vision/Picture/gzt_v3/gzt01_shrinked_2.jpg";


int main()
{
	Mat Src = imread(strSrc);				//采集所得原图像
											//Mat Template = imread(strTemplate, CV_LOAD_IMAGE_GRAYSCALE);	//用于匹配的模板图像
	if (!Src.data)
		return 0;
	Mat Dst;
	namedWindow("Src", CV_WINDOW_NORMAL);
	imshow("Src", Src);

	pyrMeanShiftFiltering(Src, Dst, 10, 20);
	imwrite("meanshiftdst3.jpg", Dst);
	namedWindow("Dst", CV_WINDOW_NORMAL);
	imshow("Dst", Dst);
	waitKey(0);
	return 0;
}

