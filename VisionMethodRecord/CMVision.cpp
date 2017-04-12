#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace std;
using namespace cv;
double Hbin = 90;
double Sbin = 40;
double Vbin = 40;
void extractHSV(Mat &image, vector<bool> &Hclass, vector<bool> &Sclass, vector<bool> &Vclass)
{
	int nChannel = image.channels();
	int nRows = image.rows;
	int nCols = image.cols;
	if (image.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *p;
	int Hindex = 0;
	int Sindex = 0;
	int Vindex = 0;
	for(int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			p = image.ptr<uchar>(i);
			Hindex = p[j*nChannel] / (180 / Hbin);
			Sindex = *(p + j*nChannel + 1) / (256 / Sbin);
			Vindex = *(p + j*nChannel + 2) / (256 / Vbin);
			Hclass[Hindex] = 1;
			Sclass[Sindex] = 1;
			Vclass[Vindex] = 1;
		}
}

void removeBack(Mat &image, vector<bool> &Hclass, vector<bool> &Sclass, vector<bool> &Vclass)
{
	int nChannel = image.channels();
	int nRows = image.rows;
	int nCols = image.cols;
	if (image.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *p;
	int Hindex = 0;
	int Sindex = 0;
	int Vindex = 0;
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			p = image.ptr<uchar>(i);
			Hindex = *(p + j*nChannel) / (180 / Hbin);
			Sindex = *(p + j*nChannel + 1) / (256 / Sbin);
			Vindex = *(p + j*nChannel + 2) / (256 / Vbin);
			if (Hclass[Hindex] && Sclass[Sindex] && Vclass[Vindex])
			{
				for (int k = 0; k < 3; k++)
					*(p + j*nChannel + k) = 0;
			}
		}
}

int main()
{
	Mat src = imread("4.bmp");
	//Mat back = imread("testback.jpg");
	Mat back = imread("1.bmp");
	cvtColor(src, src, CV_BGR2HSV);
	cvtColor(back, back, CV_BGR2HSV);
	vector<bool> Hclass(Hbin, 0);
	vector<bool> Sclass(Sbin, 0);
	vector<bool> Vclass(Vbin, 0);
	extractHSV(back, Hclass, Sclass, Vclass);
	//extractHSV(src, Hclass, Sclass, Vclass);
	removeBack(src, Hclass, Sclass, Vclass);
	cvtColor(src, src, CV_HSV2BGR);
	imshow("result", src);
	waitKey(0);
	return 0;
}