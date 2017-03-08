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
	void splitImage();						//ͼ�����Ϊ��ͨ��
	void getHist();
	void drawHist();
	int getImage(const string path);		//����ͼ��
	int getImage(const Hist src);
	void getSeg();							//��ȡͼ����ͨ���зֲ���е�ɫ��
	void scanImage(const Hist BackHist);	//����tmpHist��ɫ����ȡ��ǰ��
	void showImage();
	void initHist();
	void ThreIter();						//����������ɫ������ͼ����лҶ���ֵ�����㷨���
private:
	Mat srcimage;
	string path;
	vector<Mat> srcbgr;		//�洢������õ�BGRͨ��
	Mat histbgr[3];			//�洢BGRͨ����ֱ��ͼ����
	float range[2];			//������bgr�ĻҶȷ�Χ
	const float *ranges[1];
	int histsize;			//BGRֱ��ͼ�ĺ����귶Χ
	int histwidth, histheight;
	Mat histresult[3];

	int maxseg[3];			//��¼bgr��ͨ��ֵ���Ķ����
	int segrange;			//ɫ�׸߶�

	Mat segmask;			//��¼ɫ�ױ�������������������Ϊ��������Ϊ0
};