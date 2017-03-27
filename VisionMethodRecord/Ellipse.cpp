/*
*���ܣ���Բ���
*ʱ�䣺2014-04-19
*/ 
// Ecllipse.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include <iostream>  
using namespace cv;
using namespace std;

int sliderPos = 84;
Mat image;

//��������  
void processImage(int, void*);

int main(int argc, char** argv)
{
	const char* filename = "test1.jpg";
	//��ȡͼ��  
	image = imread(filename, 0);
	if (image.empty())
	{
		cout << "Couldn't open image " << endl;
		return 0;
	}

	imshow("source", image);
	namedWindow("result", 1);

	// Create toolbars. HighGUI use.  
	// ����һ��������  
	createTrackbar("threshold", "result", &sliderPos, 255, processImage);
	processImage(0, 0);

	// Wait for a key stroke; the same function arranges events processing  
	waitKey();
	return 0;

}

// Define trackbar callback functon. This function find contours,  
// draw it and approximate it by ellipses.  
void processImage(int /*h*/, void*)
{
	vector<vector<Point> > contours;
	//����൱�ڶ�ֵ�������matlab���Ǿ���� Iwt = Iw>=threshold;  
	Mat bimage = image >= sliderPos;
	//Mat bimage;  
	//threshold(image, bimage, sliderPos, 255,CV_THRESH_BINARY);  

	//��ȡ����,�൱��matlab����ͨ�������  
	findContours(bimage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	//���ǽ���cimage�����ͼ  
	Mat cimage = Mat::zeros(bimage.size(), CV_8UC3);

	for (size_t i = 0; i < contours.size(); i++)
	{
		//�����ı�Ե�����  
		size_t count = contours[i].size();
		//Fitzgibbon����Բ��Ϸ�����Ҫ������6���㣬���ף�Direct Least Squares Fitting of Ellipses[1999]  
		if (count < 6)
			continue;

		Mat pointsf;
		//�������еĵ�ת��Ϊ��Mat��ʽ�洢��2ά�㼯(x,y)  
		Mat(contours[i]).convertTo(pointsf, CV_32F);
		cout << pointsf.size();
		//cout << pointsf;
		//��С������ϣ�Fitzgibbon�ķ�����  
		//box��������Բ��5��������(x,y,w,h,theta)  
		RotatedRect box = fitEllipse(pointsf);

		//����Щ���������֮�Ⱥܶ����Щ��Բ�޳���  
		if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 8)
			continue;
		//��������  
		drawContours(cimage, contours, (int)i, Scalar::all(255), 1, 8);

		//������Բ  
		ellipse(cimage, box, Scalar(0, 0, 255), 1, CV_AA);
		//������Բ  
		// ellipse(cimage, box.center, box.size*0.5f, box.angle, 0, 360, Scalar(0,255,255), 1, CV_AA);  

		//���ƾ��ο�  
		Point2f vtx[4];
		//��Ա����points ���� 4�����εĶ���(x,y)  
		box.points(vtx);
		for (int j = 0; j < 4; j++)
			line(cimage, vtx[j], vtx[(j + 1) % 4], Scalar(0, 255, 0), 1, CV_AA);
	}

	imshow("result", cimage);
}