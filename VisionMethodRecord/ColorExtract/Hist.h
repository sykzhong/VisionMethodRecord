#pragma once
#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>  
#include <stdio.h>
using namespace std;
using namespace cv;
static vector<Scalar> Color = { Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(0, 0, 255) };
class Hist
{
public:
	Hist();
	void splitImage();						//图像分离为三通道
	void getHist();
	void drawHist();
	int getImage(const string path);		//输入图像
	int getImage(const Hist src);
	void getSeg();							//获取图像三通道中分布最集中的色阶
	void scanImage(const Hist BackHist);	//根据tmpHist的色阶提取出前景
	void showImage();
	void initHist();
	void ThreIter();						//对消除了有色背景的图像进行灰度阈值迭代算法求解
private:
	Mat srcimage;
	string path;
	vector<Mat> srcbgr;		//存储分离而得的BGR通道
	Mat histbgr[3];			//存储BGR通道的直方图数据
	float range[2];			//被分离bgr的灰度范围
	const float *ranges[1];
	int histsize;			//BGR直方图的横坐标范围
	int histwidth, histheight;
	Mat histresult[3];

	int maxseg[3];			//记录bgr三通道值最大的段起点
	int segrange;			//色阶高度

	Mat segmask;			//记录色阶背景消除后的掩码矩阵，若为背景掩码为0
};