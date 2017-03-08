#include "stdafx.h"
#include "Hist.h"
#include "HSVHist.h"
#define HSVHIST
int main()
{
	string srcpath = "4.bmp";
	string backpath = "1.bmp";
#ifdef RGBHIST
	Hist src, back;
	src.getImage(srcpath);
	back.getImage(backpath);
	//原图初始化
	src.initHist();
	src.drawHist();
	//背景图初始化
	back.initHist();
	back.drawHist();
	back.getSeg();
	//原图依背景色阶取前景
	src.scanImage(back);
	//back.showImage();
	//Hist tmp;
	//tmp.getImage(src);
	//tmp.initHist();
	//tmp.drawHist();
	//src.ThreIter();
	src.showImage();
	//tmp.showImage();
#else
	HSVHist src;
	src.readImage(srcpath);
	src.getHist();
	src.drawHist();
#endif
	waitKey(0);
	return 0;
}
