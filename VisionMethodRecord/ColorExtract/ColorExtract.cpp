#include "stdafx.h"
#include "Hist.h"
int main()
{
	Hist src, back;
	string srcpath = "2.bmp";
	string backpath = "1.bmp";
	src.getImage(srcpath);
	back.getImage(backpath);
	//ԭͼ��ʼ��
	src.initHist();
	src.drawHist();

	
	//����ͼ��ʼ��
	back.initHist();
	back.drawHist();
	back.getSeg();
	//ԭͼ������ɫ��ȡǰ��
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
}