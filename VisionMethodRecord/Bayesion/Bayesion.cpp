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

void calcHSComponent(Mat &image, int &Hbin, int &Sbin, vector<int> &Hclass, vector<int> &Sclass)
{
	Hclass.clear();
	Hclass.resize(180 / Hbin);
	fill(Hclass.begin(), Hclass.end(), 0);

	Sclass.clear();
	Sclass.resize(256 / Sbin);
	fill(Sclass.begin(), Sclass.end(), 0);

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
	int Sindex = 0;
	for(int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			p = image.ptr<uchar>(i);
			Hindex = p[j*nChannels] / Hbin;
			Sindex = p[j*nChannels + 1] / Sbin;
			Hclass[Hindex] += 1;
			Sclass[Sindex] += 1;
		}
}

void calcIComponent(Mat &src, int &Ibin, vector<int> &Iclass)
{
	Mat image = src.clone();
	cvtColor(image, image, CV_HSV2BGR);

	//I取值范围（-1.192, 1.192）*256，记为306
	Iclass.clear();
	Iclass.resize(306 / Ibin);
	fill(Iclass.begin(), Iclass.end(), 0);

	int nChannels = image.channels();
	int nRows = image.rows;
	int nCols = image.cols;
	if (image.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *p;
	uchar r, g, b;
	double Y, I, Q;
	int Iindex = 0;
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			p = image.ptr<uchar>(i);
			b = p[j*nChannels];
			g = p[j*nChannels + 1];
			r = p[j*nChannels + 2];
			Y = 0.299*r + 0.587*g + 0.114*b;
			I = 0.596*r - 0.275*g - 0.321*b + 153;		//I取值为-153~153，需将其转换为正数
			Q = 0.212*r - 0.523*g + 0.311*b;
			Iindex = I / Ibin;
			Iclass[Iindex] += 1;
		}
}

void saveData(int &Hbin, vector<int>&Hclass, const string &filename = "")
{
	ofstream result;
	string path = "";
	if (filename == "")
		path = "result.txt";
	else
		
		path = filename;
	result.open(path, ios::out | ios::ate);
	time_t nowtime;
	nowtime = time(NULL);
	for (int i = 0; i < Hclass.size(); i++)
	{
		result << Hbin * i << " " << Hclass[i] << endl;
	}
}

void drawData(int &Hbin, vector<int> &Hclass, const string &filename = "")
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
	string path;
	if (filename == "")
		path = "result";
	else
		path = filename;
	imshow(path, image);
}



int main()
{
	Mat src = imread("2.bmp");
	Mat back = imread("2.bmp");
	cvtColor(src, src, CV_BGR2HSV);
	int Hbin = 1;
	int Sbin = 1;
	int Ibin = 1;
	vector<int> Hclass;
	vector<int> Sclass;
	vector<int> Iclass;
	calcHSComponent(src, Hbin, Sbin, Hclass, Sclass);
	calcIComponent(src, Ibin, Iclass);
	saveData(Hbin, Hclass, "Hclass.txt");
	saveData(Sbin, Sclass, "Sclass.txt");
	saveData(Ibin, Iclass, "Iclass.txt");

	drawData(Hbin, Hclass, "Hclass.txt");
	drawData(Sbin, Sclass, "Sclass.txt");
	drawData(Ibin, Iclass, "Iclass.txt");
	waitKey(0);
	return 0;
}