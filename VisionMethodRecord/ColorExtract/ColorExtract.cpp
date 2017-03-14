#include "stdafx.h"
#include "Hist.h"
#include "HSVHist.h"
#include "ProImage.h"
#define HSVHIST
int main()
{
#ifdef RGBHIST
	string srcpath = "1.bmp";
	string backpath = "2.bmp";
	Hist src, back;
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
#else
	string srcpath = "4.bmp";
	string backpath = "1.bmp";
	ProImage src, back;
	if (!src.getImage(srcpath) || !back.getImage(backpath))
		return 0;

	src.Init();
	back.Init();
	src.removeSeg(back);
	src.showImage();

	src.preproImage();
	src.getContour();
	waitKey(0);
	
	return 0;
#endif
}
