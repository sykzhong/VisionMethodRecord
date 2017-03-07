// Test.cpp : �������̨Ӧ�ó������ڵ㡣
//ɫ�׷ֲ���ȡ

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
	void splitImage();		//ͼ�����Ϊ��ͨ��
	void getHist();
	void drawHist();
	int getImage(string path);		//����ͼ��
	void getSeg();				//��ȡͼ����ͨ���зֲ���е�ɫ��
	void scanImage();			//����ɫ����ȡ��ǰ��
	void showImage();
private:
	Mat srcimage;
	vector<Mat> srcbgr;		//�洢������õ�BGRͨ��
	Mat histbgr[3];			//�洢BGRͨ����ֱ��ͼ����
	float range[2];			//������bgr�ĻҶȷ�Χ
	const float *ranges[1];
	int histsize;			//BGRֱ��ͼ�ĺ����귶Χ
	int histwidth, histheight;
	Mat histresult[3];
	int maxseg[3];			//��¼bgr��ͨ��ֵ���Ķ����
	int segrange;			//ɫ�׸߶�
};
Hist::Hist()
{
	range[0] = 0;
	range[1] = 255;
	*ranges = &range[0];
	histsize = 255;
	histwidth = 400;
	histheight = 400;
	segrange = 20;				//ɫ�׸߶ȣ���Ҫ�ĵ��ڲ���
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
			//���ε�xyԭ�㴦��ͼ�����Ͻ�
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
	int startindex = 1;		//����ͼ���е���Ӱ������ɨ��
	//tmpsum�ĳ�ʼ��
	for (int i = 0; i < 3; i++)
		for (int j = startindex; j < seglength; j++)
		{
			val = saturate_cast<int>(histbgr[i].at<float>(j));
			tmpsum[i] += val;
		}
	//��seglengthΪ�Ƚϵ�λ�ó�ɫ���м�ֵ
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