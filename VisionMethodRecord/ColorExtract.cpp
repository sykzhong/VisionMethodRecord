// Test.cpp : 定义控制台应用程序的入口点。
//色阶分布提取

#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>  
#include <stdio.h>
using namespace std;
using namespace cv;
vector<Scalar> Color = { Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(0, 0, 255) };
class Hist
{
public:
	Hist();
	void splitImage();		//图像分离为三通道
	void getHist();
	void drawHist();
	int getImage(string path);		//输入图像
	void getSeg();				//获取图像三通道中分布最集中的色阶
	void scanImage();			//根据色阶提取出前景
	void showImage();
private:
	Mat srcimage;
	vector<Mat> srcbgr;		//存储分离而得的BGR通道
	Mat histbgr[3];			//存储BGR通道的直方图数据
	float range[2];			//被分离bgr的灰度范围
	const float *ranges[1];
	int histsize;			//BGR直方图的横坐标范围
	int histwidth, histheight;
	Mat histresult[3];
	int maxseg[3];			//记录bgr三通道值最大的段起点
	int segrange;			//色阶高度
};
Hist::Hist()
{
	range[0] = 0;
	range[1] = 255;
	*ranges = &range[0];
	histsize = 255;
	histwidth = 400;
	histheight = 400;
	segrange = 20;				//色阶高度，重要的调节参数
	for(int i = 0; i < 3; i++)
		histresult[i] = Mat(histheight, histwidth, CV_8UC3, Scalar::all(0));
}
void Hist::splitImage()
{
	split(srcimage, srcbgr);
}
void Hist::getHist()
{
	calcHist(&srcbgr[0], 1, 0, Mat(), histbgr[0], 1, &histsize, ranges);
	calcHist(&srcbgr[1], 1, 0, Mat(), histbgr[1], 1, &histsize, ranges);
	calcHist(&srcbgr[2], 1, 0, Mat(), histbgr[2], 1, &histsize, ranges);
	for (int i = 0; i < histsize; i++)
		printf("%-5d B:%-6.0f B:%-6.0f B:%-6.0f\n", i, histbgr[0].at<float>(i), histbgr[1].at<float>(i), histbgr[2].at<float>(i));
}
void Hist::drawHist()
{
	normalize(histbgr[0], histbgr[0], 0, histheight, NORM_MINMAX);
	normalize(histbgr[1], histbgr[1], 0, histheight, NORM_MINMAX);
	normalize(histbgr[2], histbgr[2], 0, histheight, NORM_MINMAX);
	for (int i = 1; i < histwidth; i++)
	{
		int val;
		for (int j = 0; j < 3; j++)
		{
			val = saturate_cast<int>(histbgr[j].at<float>(i*histsize / histwidth));
			//矩形的xy原点处在图像左上角
			rectangle(histresult[j], Point(i - 1, histheight), Point(i, histheight - val), Color[j]);
		}
	}
	for (int i = 0; i < 3; i++)
	{
		char name[200];
		sprintf(name, "num.%d", i);
		imshow(name, histresult[i]);
	}
}
int Hist::getImage(string path)
{
	srcimage = imread(path, 1);
	if (!srcimage.data)
		return 0;
	return 1;
}
void Hist::getSeg()
{
	vector<int> MAXsum(3, -1);
	int tmpsum[3] = { 0 };
	int seglength = 10;
	int val;
	int startindex = 1;		//跳过图像中的阴影部分再扫描
	//tmpsum的初始化
	for (int i = 0; i < 3; i++)
		for (int j = startindex; j < seglength; j++)
		{
			val = saturate_cast<int>(histbgr[i].at<float>(j));
			tmpsum[i] += val;
		}
	//以seglength为比较单位得出色阶中间值
	for (int i = 0; i < 3; i++)
	{
		for (int j = startindex; j < histsize - seglength; j++)
		{
			if (tmpsum[i] > MAXsum[i])
			{
				MAXsum[i] = tmpsum[i];
				maxseg[i] = j+seglength/2;
			}
			val = saturate_cast<int>(histbgr[i].at<float>(j));
			tmpsum[i] -= val;
			val = saturate_cast<int>(histbgr[i].at<float>(j+seglength));
			tmpsum[i] += val;
		}
	}
}
void Hist::scanImage()
{
	cout << srcimage.depth() << " " << sizeof(uchar);
	CV_Assert(srcimage.depth() == 0);
	int channels = srcimage.channels();
	int nRows = srcimage.rows, nCols = srcimage.cols;
	if (srcimage.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *p;
	for (int i = 0; i < nRows; i++)
	{
		p = srcimage.ptr<uchar>(i);
		for (int j = 0; j < nCols; j++)
		{
			int sign = 1;
			int k = 0;
			for (k = 0; k < 3; k++)
			{
				if (p[j*channels + k] < maxseg[k] - segrange || p[j*channels + k] > maxseg[k] + segrange)
				{
					sign = 0;
					break;
				}
			}
			if (sign == 1)
				for (k = 0; k < 3; k++)
					p[j*channels + k] = 255;
		}
	}
}
void Hist::showImage()
{ 
	imshow("result", srcimage);
}
int main()
{
	Hist src;
	string path = "wp_000.bmp";
	src.getImage(path);
	src.splitImage();
	src.getHist();
	src.drawHist();
	src.getSeg();
	src.scanImage();
	src.showImage();
	waitKey(0);
	return 0;
}