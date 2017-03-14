#include "stdafx.h"
#include "HSVHist.h"
HSVHist::HSVHist()
{
	histsize[0] = 8;		//Hͨ��bin��
	histsize[1] = 40;		//Sͨ��
	histsize[2] = 8;		//Vͨ��

	hrange[0] = 0;			//��tmbug��������й��оص���Ϊ180�����
	hrange[1] = 180;
	ranges[0] = hrange;

	svrange[0] = 0;
	svrange[1] = 255;
	for (int i = 1; i < 3; i++)
		ranges[i] = svrange;		//��ͨ��ȡֵ��Χ
	
	bin_w = 6;
	hist_h = 240;
	hist_w = histsize[0]*histsize[1] *bin_w;	//hs��bin���ܺͣ������Ϊ6
	hist_img = Mat(hist_h, hist_w, CV_8UC3, Scalar::all(0));
}

HSVHist::~HSVHist()
{

}

int HSVHist::getImage(string path)
{
	srcimage = imread(path, 1);
	if (!srcimage.data)
		return 0;
	this->path = path;
	cvtColor(srcimage, srcimage, COLOR_BGR2HSV);
	return 1;
}

void HSVHist::Init()
{
	medianBlur(srcimage, srcimage, 3);
	getHist();
	drawHist();
}

void HSVHist::getHist()
{
	split(srcimage, hsvplane);			//ͨ������
	int channels[3] = { 0, 1, 2};
	calcHist(hsvplane, 3, channels, Mat(), hsvhist, 2, histsize, ranges);		//ֱ��ͼ����
	//��һ������
	minMaxLoc(hsvhist, 0, &maxval, 0, 0);
	for (int i = 0; i < histsize[0]; i++)
		for (int j = 0; j < histsize[1]; j++)
		{
			float binval = hsvhist.at<float>(i, j);
			binval = round(binval*hist_h / maxval);
			hsvhist.at<float>(i, j) = binval;
		}
}

void HSVHist::drawHist()
{
	Mat hsv_color = Mat(1, 1, CV_8UC3);
	Mat rgb_color = Mat(1, 1, CV_8UC3);
	for(int i = 0; i < histsize[0]; i++)
		for (int j = 0; j < histsize[1]; j++)
		{
			int loc = i*histsize[1] + j;
			float binval = hsvhist.at<float>(i, j);
			//printf("i:%d j:%d binval:%.0f\n", i, j, binval);
			
			//��ɫ��hsvת��Ϊrgb
			hsv_color.setTo(Scalar(i*179.f / histsize[0], j*255.f / histsize[1], 255, 0));
			cvtColor(hsv_color, rgb_color, COLOR_HSV2BGR);
			Scalar color = rgb_color.at<Vec3b>(0, 0);
			
			rectangle(hist_img, Point(loc*bin_w, hist_h), Point((loc + 1)*bin_w, hist_h - binval), color, -1);
		}
	imshow(path, hist_img);
}

void HSVHist::removeSeg(HSVHist back)
{
	int nRows = srcimage.rows;
	int nCols = srcimage.cols;
	int nChannels = srcimage.channels();
	if(srcimage.isContinuous() == true)
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *psrc;
	int Hval, Sval;
	for(int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			//��ȡԭͼ�е�ɫ��
			psrc = srcimage.ptr<uchar>(i);
			Hval = psrc[j*nChannels]*histsize[0]/180;
			Sval = psrc[j*nChannels + 1]*histsize[1]/256;

			if (back.hsvhist.at<float>(Hval, Sval) > 0)
				for (int k = 0; k < 3; k++)
					psrc[j*nChannels + k] = 0;
		}
	cvtColor(srcimage, srcimage, CV_HSV2BGR);
}

void HSVHist::showImage(string strpath)
{
	if (strpath == "")
		strpath = this->path;
	strpath += "hist";
	imshow(strpath, srcimage);
	imwrite("result.jpg", srcimage);
}
