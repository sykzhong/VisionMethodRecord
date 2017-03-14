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
class ProImage;
class HSVHist
{
public:
	HSVHist();
	~HSVHist();
	int getImage(string path);
	void getHist();
	void drawHist();
	void removeSeg(HSVHist back);
	void showImage(string strpath = "");
	void Init();
private:
	Mat hsvplane[3];		//存储HSV三通道
	
	float svrange[2];
	float hrange[2];
	const float *ranges[3];	//hsv的取值范围
	double maxval;			//原始数据中的最大值

	int bin_w;				//直方图单格宽度
	int hist_h, hist_w;		//直方图高与宽
	Mat hist_img;			//用于显示处理后的直方图

protected:
	string path;			//原图路径

	Mat srcimage;			//原图像,读取为HSV格式
	Mat hsvhist;			//用于存储直方图计算结果
	int histsize[3];		//hsv在直方图中分布数
};


#endif

