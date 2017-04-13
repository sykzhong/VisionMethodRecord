#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <time.h>
#include <algorithm>
#include <fstream>
using namespace cv;
using namespace std;

static const Scalar RED = Scalar(0, 0, 255);
static const Scalar PINK = Scalar(230, 130, 255);
static const Scalar BLUE = Scalar(255, 0, 0);
static const Scalar LIGHTBLUE = Scalar(255, 255, 160);
static const Scalar GREEN = Scalar(0, 255, 0);
static const Scalar BLACK = Scalar(0, 0, 0);
static const Scalar WHITE = Scalar(255, 255, 255);

void calcHSComponent(Mat &image, int &Hbin, vector<int> &Hclass)
{
	Hclass.clear();
	Hclass.resize(180 / Hbin);
	fill(Hclass.begin(), Hclass.end(), 0);
	int nChannels = image.channels();
	int nRows = image.rows;
	int nCols = image.cols;
	if (image.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *p;
	int Hindex = 0;
	for(int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			p = image.ptr<uchar>(i);
			Hindex = p[j*nChannels] / Hbin;
			Hclass[Hindex] += 1;
		}
}

void saveData(int &Hbin, vector<int>&Hclass)
{
	ofstream result;
	result.open("Hclass.txt", ios::out | ios::ate);
	time_t nowtime;
	nowtime = time(NULL);
	for (int i = 0; i < Hclass.size(); i++)
	{
		result << Hbin * i << " " << Hclass[i] << endl;
	}
}

void drawData(int &Hbin, vector<int> &Hclass)
{
	//绘制的图像高,宽,单格宽
	int height = 400;
	double hratio = 0.8;		//最高者只能占总高的比例
	//int width = 400;
	//int w_bin = round(width / Hclass.size());
	int w_bin = 2;
	int width = w_bin*Hclass.size();
	Mat image(height, width, CV_8UC3);
	
	int maxheight = *max_element(Hclass.begin(), Hclass.end());
	for (int i = 0; i < Hclass.size(); i++)
	{
		int tmpheight = round(Hclass[i] * (height*hratio) / maxheight);
		circle(image, Point(w_bin*(i + 1), height - tmpheight), 2, RED, -1);
		if (i != 0)
		{
			int pretmpheight = round(Hclass[i - 1] * (height*hratio) / maxheight);
			line(image, Point(w_bin*(i + 1), height - tmpheight), Point(w_bin*i, height - pretmpheight), RED, 1);
		}
			
	}
	imshow("Hclass", image);
}

int main()
{
	Mat src = imread("1.bmp");
	cvtColor(src, src, CV_BGR2HSV);
	int Hbin = 1;
	vector<int> Hclass;
	calcHSComponent(src, Hbin, Hclass);
	saveData(Hbin, Hclass);
	drawData(Hbin, Hclass);
	waitKey(0);
	return 0;
}