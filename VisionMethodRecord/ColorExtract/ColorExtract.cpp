#include "stdafx.h"
#include "Hist.h"
int main()
{
	Hist src, back;
	string srcpath = "4.bmp";
	string backpath = "1.bmp";
	src.getImage(srcpath);
	back.getImage(backpath);
	//原图初始化
	src.initHist();
	//src.drawHist();
	//背景图初始化
	back.initHist();
	back.drawHist();
	back.getSeg();
	//原图依背景色阶取前景
	src.scanImage(back);
	src.showImage();
	waitKey(0);
	return 0;
}