#include "stdafx.h"
#include "HSVHist.h"
HSVHist::HSVHist()
{
	histsize[0] = 16;		//Hͨ��bin��
	histsize[1] = 8;		//Sͨ��
	histsize[2] = 8;		//Vͨ��

	hrange[0] = 0;			//��tmbug��������й��оص���Ϊ180�ᱨ��
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
int HSVHist::readImage(string path)
{
	srcimage = imread(path, 1);
	if (!srcimage.data)
		return 0;
	cvtColor(srcimage, srcimage, COLOR_BGR2HSV);
	return 1;
}
void HSVHist::getHist()
{
	split(srcimage, hsvplane);			//ͨ������
	int channels[3] = { 0, 1};
	calcHist(hsvplane, 2, channels, Mat(), hsvhist, 2, histsize, ranges);		//ֱ��ͼ����
	minMaxLoc(hsvhist, 0, &maxval, 0, 0);
}

void HSVHist::drawHist()
{
	Mat hsv_color = Mat(1, 1, CV_8UC3);
	Mat rgb_color = Mat(1, 1, CV_8UC3);
	cout << hsvhist << endl;
	for(int i = 0; i < histsize[0]; i++)
		for (int j = 0; j < histsize[1]; j++)
		{
			int loc = i*histsize[1] + j;
			float binval = hsvhist.at<float>(i, j);
			cout << i << " " << " " << j << " " << binval << endl;
			binval = round(binval*hist_h / maxval);
			
			//��ɫ��hsvת��Ϊrgb
			hsv_color.setTo(Scalar(i*180.f / histsize[0], j*255.f / histsize[1], 255, 0));
			cvtColor(hsv_color, rgb_color, COLOR_HSV2BGR);
			Scalar color = rgb_color.at<Vec3b>(0, 0);
			
			rectangle(hist_img, Point(loc*bin_w, hist_h), Point((loc + 1)*bin_w, hist_h - binval), color, -1);
		}
	imshow("hsvhist", hist_img);
}