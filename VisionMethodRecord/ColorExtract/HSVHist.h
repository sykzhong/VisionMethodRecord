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
	Mat srcimage;			//原图像 
	Mat hsvplane[3];		//存储HSV三通道
	int histsize[3];		//hsv在直方图中分布数
	float svrange[2];
	float hrange[2];
	const float *ranges[3];		//hsv的取值范围

	Mat hsvhist;			//用于存储直方图计算结果
	double maxval;				//原始数据中的最大值

	int bin_w;				//直方图单格宽度
	int hist_h, hist_w;
	Mat hist_img;		//用于显示处理后的直方图
};


#endif

