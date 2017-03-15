#ifndef _HSVHIST_H_
#define _HSVHIST_H_
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>  
#include <stdio.h>
using namespace std;
using namespace cv;

const Scalar RED = Scalar(0, 0, 255);
const Scalar PINK = Scalar(230, 130, 255);
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar LIGHTBLUE = Scalar(255, 255, 160);
const Scalar GREEN = Scalar(0, 255, 0);

class HSVHist
{
public:
	HSVHist();
	~HSVHist();
	int getImage(string path);
	void getHist();
	void drawHist();
	void removeSeg(HSVHist back);
	static void removeBack(HSVHist dst, HSVHist back);
	void showImage(string strpath = "");
	void Init();
private:
	Mat hsvplane[3];		//�洢HSV��ͨ��
	
	float svrange[2];
	float hrange[2];
	const float *ranges[3];	//hsv��ȡֵ��Χ
	double maxval;			//ԭʼ�����е����ֵ

	int bin_w;				//ֱ��ͼ������
	int hist_h, hist_w;		//ֱ��ͼ�����
	Mat hist_img;			//������ʾ������ֱ��ͼ

protected:
	string path;			//ԭͼ·��

	Mat srcimage;			//ԭͼ��,��ȡΪHSV��ʽ
	Mat m_image;
	Mat hsvhist;			//���ڴ洢ֱ��ͼ������
	int histsize[3];		//hsv��ֱ��ͼ�зֲ���
};


#endif

