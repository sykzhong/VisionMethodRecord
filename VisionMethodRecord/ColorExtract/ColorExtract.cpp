#include "stdafx.h"
#include "Hist.h"
int main()
{
	Hist src, back;
	string srcpath = "4.bmp";
	string backpath = "1.bmp";
	src.getImage(srcpath);
	back.getImage(backpath);
	//ԭͼ��ʼ��
	src.initHist();
	//src.drawHist();
	//����ͼ��ʼ��
	back.initHist();
	back.drawHist();
	back.getSeg();
	//ԭͼ������ɫ��ȡǰ��
	src.scanImage(back);
	src.showImage();
	waitKey(0);
	return 0;
}