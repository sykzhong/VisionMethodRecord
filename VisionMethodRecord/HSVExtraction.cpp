// HSVExtraction.cpp : 定义控制台应用程序的入口点。
//利用HSV模型对图像进行分割，最终提取得到二值图像

#include "stdafx.h"
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

const Scalar RED = Scalar(0, 0, 255);
const Scalar PINK = Scalar(230, 130, 255);
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar LIGHTBLUE = Scalar(255, 255, 160);
const Scalar GREEN = Scalar(0, 255, 0);
const Scalar BLACK = Scalar(0, 0, 0);

/*各种颜色的H取值范围
Orange 0 - 22 Yellow 22 - 38 Green 38 - 75 Blue 75 - 130 Violet 130 - 160 Red 160 - 179
*/
const int msfsp = 30;		//作为meanshift filter的像素位置差值
const int msfsr = 20;		//作为Meanshift filter的像素大小差值

const char* controlwin = "Control";			//控制窗口名称
const char* srcwin = "imgsrc";
const char* dstwin = "imgdst";
const char* contourwin = "contour";

Mat image;
Mat imgOriginal;		//全局变量，滤波完成后的初始图像，用于trackbar
Mat imgDst;				//全局变量，HSV提取后的阈值图像，用于trackbar
Mat imgContour;			//全局变量，HSV提取后的轮廓，用于trackbar

						///HSV值
int iLowH = 0;
int iHighH = 70;

int iLowS = 0;
int iHighS = 255;

int iLowV = 0;
int iHighV = 255;

void HSVExtraction(int, void*);

bool compareArea(vector<Point> _first, vector<Point> _second)
{
	float area1, area2;
	area1 = contourArea(_first, false);
	area2 = contourArea(_second, false);
	if (area1 > area2)
		return true;
	else return false;
}

int main(int argc, char** argv)
{
	image = imread("E:\\Cloud\\Research\\Vision\\VisionMethodRecord\\Picture\\wp_01.bmp"); // read a new frame from video
	if (!image.data)  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}
	//pyrMeanShiftFiltering(image, imgOriginal, msfsp, msfsr);		//进行meanshift滤波
	imgOriginal = image.clone();


	namedWindow(controlwin, CV_WINDOW_NORMAL); //create a window called "Control"
	namedWindow(srcwin, CV_WINDOW_NORMAL);


	namedWindow(dstwin, CV_WINDOW_NORMAL);
	namedWindow(contourwin, CV_WINDOW_NORMAL);
	imshow(srcwin, image);
	//Create trackbars in "Control" window
	createTrackbar("LowH", controlwin, &iLowH, 179, HSVExtraction); //Hue (0 - 179)
	createTrackbar("HighH", controlwin, &iHighH, 179, HSVExtraction);

	createTrackbar("LowS", controlwin, &iLowS, 255, HSVExtraction); //Saturation (0 - 255)
	createTrackbar("HighS", controlwin, &iHighS, 255, HSVExtraction);

	createTrackbar("LowV", controlwin, &iLowV, 255, HSVExtraction); //Value (0 - 255)
	createTrackbar("HighV", controlwin, &iHighV, 255, HSVExtraction);

	HSVExtraction(0, 0);				//缺省第一次执行
	waitKey(0);
}

void HSVExtraction(int, void*)
{
	vector<Mat> hsvSplit;
	Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

												  //因为我们读取的是彩色图，直方图均衡化需要在HSV空间做
	split(imgHSV, hsvSplit);
	equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, imgHSV);
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgDst);

	vector<vector<Point>> contours;		//用于存储原图像和模板图像的所有轮廓
	vector<Vec4i> hierarchy;
	findContours(imgDst, contours, hierarchy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	if (contours.size() <= 0)
	{
		cout << "failed to search the workpiece" << endl;
		return;
	}

	sort(contours.begin(), contours.end(), compareArea);

	vector<Point> Contour;
	Contour = *(contours.begin());		//选择其中面积第二大的轮廓作为工件轮廓

	imgContour = image.clone();			//对源图像进行复制
	vector<vector<Point>> vectemp;				//用于轮廓描绘
	vectemp.clear();
	vectemp.push_back(Contour);
	drawContours(imgContour, vectemp, -1, RED);

	//imgContour = image.clone();			//对源图像进行复制
	//drawContours(imgContour, contours, -1, RED);

	imshow(dstwin, imgDst);
	imshow(contourwin, imgContour);
	imwrite("contours.jpg", imgContour);
}