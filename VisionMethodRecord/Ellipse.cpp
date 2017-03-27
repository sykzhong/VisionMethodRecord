/*
*功能：椭圆拟合
*时间：2014-04-19
*/ 
// Ecllipse.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include <iostream>  
using namespace cv;
using namespace std;

int sliderPos = 84;
Mat image;

//函数声明  
void processImage(int, void*);

int main(int argc, char** argv)
{
	const char* filename = "test1.jpg";
	//读取图像  
	image = imread(filename, 0);
	if (image.empty())
	{
		cout << "Couldn't open image " << endl;
		return 0;
	}

	imshow("source", image);
	namedWindow("result", 1);

	// Create toolbars. HighGUI use.  
	// 创建一个滑动块  
	createTrackbar("threshold", "result", &sliderPos, 255, processImage);
	processImage(0, 0);

	// Wait for a key stroke; the same function arranges events processing  
	waitKey();
	return 0;

}

// Define trackbar callback functon. This function find contours,  
// draw it and approximate it by ellipses.  
void processImage(int /*h*/, void*)
{
	vector<vector<Point> > contours;
	//这句相当于二值化。这个matlab的那句好像： Iwt = Iw>=threshold;  
	Mat bimage = image >= sliderPos;
	//Mat bimage;  
	//threshold(image, bimage, sliderPos, 255,CV_THRESH_BINARY);  

	//提取轮廓,相当于matlab中连通区域分析  
	findContours(bimage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	//我们将在cimage上面绘图  
	Mat cimage = Mat::zeros(bimage.size(), CV_8UC3);

	for (size_t i = 0; i < contours.size(); i++)
	{
		//轮廓的边缘点个数  
		size_t count = contours[i].size();
		//Fitzgibbon的椭圆拟合方法，要求至少6个点，文献：Direct Least Squares Fitting of Ellipses[1999]  
		if (count < 6)
			continue;

		Mat pointsf;
		//将轮廓中的点转换为以Mat形式存储的2维点集(x,y)  
		Mat(contours[i]).convertTo(pointsf, CV_32F);
		cout << pointsf.size();
		//cout << pointsf;
		//最小二次拟合（Fitzgibbon的方法）  
		//box包含了椭圆的5个参数：(x,y,w,h,theta)  
		RotatedRect box = fitEllipse(pointsf);

		//把那些长轴与短轴之比很多的那些椭圆剔除。  
		if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 8)
			continue;
		//绘制轮廓  
		drawContours(cimage, contours, (int)i, Scalar::all(255), 1, 8);

		//绘制椭圆  
		ellipse(cimage, box, Scalar(0, 0, 255), 1, CV_AA);
		//绘制椭圆  
		// ellipse(cimage, box.center, box.size*0.5f, box.angle, 0, 360, Scalar(0,255,255), 1, CV_AA);  

		//绘制矩形框  
		Point2f vtx[4];
		//成员函数points 返回 4个矩形的顶点(x,y)  
		box.points(vtx);
		for (int j = 0; j < 4; j++)
			line(cimage, vtx[j], vtx[(j + 1) % 4], Scalar(0, 255, 0), 1, CV_AA);
	}

	imshow("result", cimage);
}