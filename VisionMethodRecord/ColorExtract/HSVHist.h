#define _HSVHIST_H_
#ifdef _HSVHIST_H_
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>  
#include <stdio.h>
using namespace std;
using namespace cv;
class HSVHist
{
public:
	HSVHist();
	int readImage(string path);
	void getHist();
	void drawHist();
private:
	Mat srcimage;			//ԭͼ�� 
	Mat hsvplane[3];		//�洢HSV��ͨ��
	int histsize[3];		//hsv��ֱ��ͼ�зֲ���
	float svrange[2];
	float hrange[2];
	const float *ranges[3];		//hsv��ȡֵ��Χ

	Mat hsvhist;			//���ڴ洢ֱ��ͼ������
	double maxval;				//ԭʼ�����е����ֵ

	int bin_w;				//ֱ��ͼ������
	int hist_h, hist_w;
	Mat hist_img;		//������ʾ������ֱ��ͼ
};


#endif

