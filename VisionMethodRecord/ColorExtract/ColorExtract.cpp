#include "stdafx.h"
#include "Hist.h"
#include "HSVHist.h"
#define HSVHIST
int main()
{
#ifdef RGBHIST
	string srcpath = "1.bmp";
	string backpath = "2.bmp";
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
	waitKey(0);
	return 0;
#else
	string srcpath = "2.bmp";
	string backpath = "1.bmp";
	HSVHist src, back;
	if (!src.readImage(srcpath) || !back.readImage(backpath))
		return 0;

	src.Init();
	back.Init();

	src.removeSeg(back);
	src.showImage();
	waitKey(0);
	return 0;
#endif
}
