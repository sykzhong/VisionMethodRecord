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

int Hbin = 1;
int Sbin = 1;
int Ibin = 1;
vector<int> Hclass;
vector<int> Sclass;
vector<int> Iclass;

void calcHSComponent(Mat &image, int &_Hbin, int &_Sbin, vector<int> &_Hclass, vector<int> &_Sclass)
{
	_Hclass.clear();
	_Hclass.resize(180 / _Hbin);
	fill(_Hclass.begin(), _Hclass.end(), 0);

	_Sclass.clear();
	_Sclass.resize(256 / _Sbin);
	fill(_Sclass.begin(), _Sclass.end(), 0);

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
			Hindex = p[j*nChannels] / _Hbin;
			Sindex = p[j*nChannels + 1] / _Sbin;
			_Hclass[Hindex] += 1;
			_Sclass[Sindex] += 1;
		}
}

void calcIComponent(Mat &src, int &_Ibin, vector<int> &_Iclass)
{
	Mat image = src.clone();
	cvtColor(image, image, CV_HSV2BGR);

	//I取值范围（-1.192, 1.192）*256，记为306
	_Iclass.clear();
	_Iclass.resize(306 / _Ibin);
	fill(_Iclass.begin(), _Iclass.end(), 0);

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
			Iindex = I / _Ibin;
			_Iclass[Iindex] += 1;
		}
}

void saveData(int &_Hbin, vector<int>&_Hclass, const string &filename = "")
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
	for (int i = 0; i < _Hclass.size(); i++)
	{
		result << _Hbin * i << " " << _Hclass[i] << endl;
	}
}

void drawData(int &_Hbin, vector<int> &_Hclass, const string &filename = "")
{
	//绘制的图像高,宽,单格宽
	int height = 400;
	double hratio = 0.8;		//最高者只能占总高的比例
	//int width = 400;
	//int w_bin = round(width / _Hclass.size());
	int w_bin = 2;
	int width = w_bin*_Hclass.size();
	Mat image(height, width, CV_8UC3);
	
	int maxheight = *max_element(_Hclass.begin(), _Hclass.end());
	for (int i = 0; i < _Hclass.size(); i++)
	{
		int tmpheight = round(_Hclass[i] * (height*hratio) / maxheight);
		circle(image, Point(w_bin*(i + 1), height - tmpheight), 2, RED, -1);
		if (i != 0)
		{
			int pretmpheight = round(_Hclass[i - 1] * (height*hratio) / maxheight);
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

void getTrainingData(Mat &image, Mat &trainingData)			//image为HSV图像
{
	int nRows = image.rows;
	int nCols = image.cols;
	trainingData = Mat(image.rows, image.cols, CV_8UC2);
	int imageChannels = image.channels();
	int trainChannels = trainingData.channels();
	if (image.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	Mat bgrimage = Mat(image.size(), CV_8UC3);
	cvtColor(image, bgrimage, CV_HSV2BGR);
	uchar *pimage;			//原图像的行指针
	uchar *pbgrimage;		//转换为BGR图像的行指针
	uchar *ptrain;			//
	int b, g, r;
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			pimage = image.ptr<uchar>(i);
			pbgrimage = bgrimage.ptr<uchar>(i);
			ptrain = trainingData.ptr<uchar>(i);
			b = pbgrimage[j*imageChannels];
			g = pbgrimage[j*imageChannels + 1];
			r = pbgrimage[j*imageChannels + 2];
			ptrain[j*trainChannels] = pimage[j*imageChannels] / Hbin;
			//ptrain[j*trainChannels + 1] = pimage[j*imageChannels + 1] / Sbin;
			ptrain[j*trainChannels + 1] = 0.596*r - 0.275*g - 0.321*b;
		}
}

int main()
{
	Mat src = imread("2.bmp");
	Mat back = imread("2.bmp");
	cvtColor(src, src, CV_BGR2HSV);

	calcHSComponent(src, Hbin, Sbin, Hclass, Sclass);
	calcIComponent(src, Ibin, Iclass);
	saveData(Hbin, Hclass, "Bayesion/Hclass.txt");
	saveData(Sbin, Sclass, "Bayesion/Sclass.txt");
	saveData(Ibin, Iclass, "Bayesion/Iclass.txt");

	drawData(Hbin, Hclass, "Bayesion/Hclass.txt");
	drawData(Sbin, Sclass, "Bayesion/Sclass.txt");
	drawData(Ibin, Iclass, "Bayesion/Iclass.txt");
	waitKey(0);
	return 0;
}