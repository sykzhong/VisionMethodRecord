// Test.cpp : 定义控制台应用程序的入口点。
//色阶分布提取
#include "stdafx.h"
#include "Hist.h"
Hist::Hist()
{
	//RGB直方图计算的相关参数：
	range[0] = 0;
	range[1] = 255;
	*ranges = &range[0];
	histsize = 255;
	histwidth = 400;
	histheight = 400;

	segrange = 40;				//色阶高度，重要的调节参数
	for (int i = 0; i < 3; i++)
	{
		maxseg[i] = 0;			//最高色阶初始化
		histresult[i] = Mat(histheight, histwidth, CV_8UC3, Scalar::all(0));	//RGB三通道直方图绘制初始化
	}
}
void Hist::splitImage()
{
	split(srcimage, srcbgr);
}
void Hist::getHist()
{
	for (int i = 0; i < 3; i++)
	{
		calcHist(&srcbgr[i], 1, 0, Mat(), histbgr[i], 1, &histsize, ranges);
		normalize(histbgr[i], histbgr[i], 0, histheight, NORM_MINMAX);
	}
	for (int i = 0; i < histsize; i++)
		printf("%-5d B:%-6.0f B:%-6.0f B:%-6.0f\n", i, histbgr[0].at<float>(i), histbgr[1].at<float>(i), histbgr[2].at<float>(i));
}
void Hist::drawHist()
{
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

		sprintf(name, "%s:num.%d", path.c_str(), i);
		imshow(name, histresult[i]);
	}
}
int Hist::getImage(const string path)
{
	srcimage = imread(path, 1);
	if (!srcimage.data)
		return 0;
	this->path = path;
	return 1;
}
int Hist::getImage(const Hist src)
{
	if (!src.srcimage.data)
		return 0;
	src.srcimage.copyTo(srcimage);
	this->path = "tmp" + src.path;
	return 1;
}
void Hist::getSeg()
{
	vector<int> MAXsum(3, -1);
	int tmpsum[3] = { 0 };
	int seglength = 10;
	int val;
	int startindex = 1;		//跳过图像中的阴影部分再扫描?
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
void Hist::scanImage(const Hist BackHist)
{
	CV_Assert(srcimage.depth() == 0);
	int channels = srcimage.channels();
	int nRows = srcimage.rows, nCols = srcimage.cols;
	if (srcimage.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	//提取出背景的直方图分布
	vector<int> tmpmaxseg(3);
	for (int i = 0; i < 3; i++)
		tmpmaxseg[i] = BackHist.maxseg[i];
	uchar *p, *q;							//用于记录原图行头、掩码行头的指针
	segmask = Mat(srcimage.rows, srcimage.cols, CV_8U, Scalar::all(255));		//背景去除掩码初始化
	for (int i = 0; i < nRows; i++)
	{
		p = srcimage.ptr<uchar>(i);		//src行头坐标
		q = segmask.ptr<uchar>(i);		//segmask行头坐标
		for (int j = 0; j < nCols; j++)
		{
			int sign = 1;
			int k = 0;
			for (k = 0; k < 3; k++)
			{
				if (p[j*channels + k] < tmpmaxseg[k] - segrange || p[j*channels + k] > tmpmaxseg[k] + segrange)
				{
					sign = 0;
					break;
				}
			}
			if (sign == 1)
				for (k = 0; k < 3; k++)
					p[j*channels + k] = 0;
			if (sign == 1)
				q[j] = 0;
		}
	}
}
void Hist::showImage()
{ 
	imshow("result", srcimage);
	imshow("mask", segmask);
}
void Hist::initHist()
{
	this->splitImage();
	this->getHist();
}
void Hist::ThreIter()
{
	
	Mat imageTemp;
	imageTemp = srcimage.clone();
	cvtColor(imageTemp, imageTemp, CV_RGB2GRAY);
	//imageTemp.reshape(1, 0);						//将该图像转换为单通道

	double Zavr, Zmin, Zmax;
	//int *minIdx, *maxIdx;
	Point Pmin, Pmax;

	minMaxLoc(imageTemp, &Zmin, &Zmax, &Pmin, &Pmax, segmask);
	imshow("imageTemp", imageTemp);
	Zavr = (Zmin + Zmax) / 2;						//求出图像平均灰度

	Mat imageFore, imageBack;						//定义图像前景与背景，前景灰度低
	double thTemp;
	int itercount = 1;
	while (1)
	{
		threshold(imageTemp, imageFore, Zavr, 255, THRESH_TRUNC);
		imageBack = imageTemp - imageFore;
		//求取前景后景的平均灰度
		Zmin = mean(imageFore, segmask)[0];
		Zmax = mean(imageBack, segmask)[0];
		thTemp = (Zmin + Zmax) / 2;

		if ((abs(thTemp - Zavr) < 0.5) || itercount >= 100)
			break;
		else
		{
			Zavr = thTemp;
			itercount++;
		}
	}
	threshold(imageTemp, imageFore, Zavr*3, 255, THRESH_TOZERO_INV);
	imageFore.copyTo(srcimage, segmask);
	//threshold(srcimage, srcimage, Zavr, 255, THRESH_BINARY);

	//threshold(image, image, Zavr, maxThValue, THRESH_BINARY);
	return;
}