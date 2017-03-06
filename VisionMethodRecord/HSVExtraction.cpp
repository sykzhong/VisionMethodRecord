// HSVExtraction.cpp : �������̨Ӧ�ó������ڵ㡣
//����HSVģ�Ͷ�ͼ����зָ������ȡ�õ���ֵͼ��

#include "stdafx.h"
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

const Scalar RED = Scalar(0, 0, 255);
const Scalar PINK = Scalar(230, 130, 255);
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar LIGHTBLUE = Scalar(255, 255, 160);
const Scalar GREEN = Scalar(0, 255, 0);
const Scalar BLACK = Scalar(0, 0, 0);

/*������ɫ��Hȡֵ��Χ
Orange 0 - 22 Yellow 22 - 38 Green 38 - 75 Blue 75 - 130 Violet 130 - 160 Red 160 - 179
*/
const int msfsp = 30;		//��Ϊmeanshift filter������λ�ò�ֵ
const int msfsr = 20;		//��ΪMeanshift filter�����ش�С��ֵ

const char* controlwin = "Control";			//���ƴ�������
const char* srcwin = "imgsrc";
const char* dstwin = "imgdst";
const char* contourwin = "contour";

Mat image;
Mat imgOriginal;		//ȫ�ֱ������˲���ɺ�ĳ�ʼͼ������trackbar
Mat imgDst;				//ȫ�ֱ�����HSV��ȡ�����ֵͼ������trackbar
Mat imgContour;			//ȫ�ֱ�����HSV��ȡ�������������trackbar

						///HSVֵ
int iLowH = 0;
int iHighH = 70;

int iLowS = 0;
int iHighS = 255;

int iLowV = 0;
int iHighV = 255;

void HSVExtraction(int, void*);

bool compareArea(vector<Point> _first, vector<Point> _second)
{
	float area1, area2;
	area1 = contourArea(_first, false);
	area2 = contourArea(_second, false);
	if (area1 > area2)
		return true;
	else return false;
}

int main(int argc, char** argv)
{
	image = imread("E:\\Cloud\\Research\\Vision\\VisionMethodRecord\\Picture\\wp_01.bmp"); // read a new frame from video
	if (!image.data)  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}
	//pyrMeanShiftFiltering(image, imgOriginal, msfsp, msfsr);		//����meanshift�˲�
	imgOriginal = image.clone();


	namedWindow(controlwin, CV_WINDOW_NORMAL); //create a window called "Control"
	namedWindow(srcwin, CV_WINDOW_NORMAL);


	namedWindow(dstwin, CV_WINDOW_NORMAL);
	namedWindow(contourwin, CV_WINDOW_NORMAL);
	imshow(srcwin, image);
	//Create trackbars in "Control" window
	createTrackbar("LowH", controlwin, &iLowH, 179, HSVExtraction); //Hue (0 - 179)
	createTrackbar("HighH", controlwin, &iHighH, 179, HSVExtraction);

	createTrackbar("LowS", controlwin, &iLowS, 255, HSVExtraction); //Saturation (0 - 255)
	createTrackbar("HighS", controlwin, &iHighS, 255, HSVExtraction);

	createTrackbar("LowV", controlwin, &iLowV, 255, HSVExtraction); //Value (0 - 255)
	createTrackbar("HighV", controlwin, &iHighV, 255, HSVExtraction);

	HSVExtraction(0, 0);				//ȱʡ��һ��ִ��
	waitKey(0);
}

void HSVExtraction(int, void*)
{
	vector<Mat> hsvSplit;
	Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

												  //��Ϊ���Ƕ�ȡ���ǲ�ɫͼ��ֱ��ͼ���⻯��Ҫ��HSV�ռ���
	split(imgHSV, hsvSplit);
	equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, imgHSV);
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgDst);

	vector<vector<Point>> contours;		//���ڴ洢ԭͼ���ģ��ͼ�����������
	vector<Vec4i> hierarchy;
	findContours(imgDst, contours, hierarchy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	if (contours.size() <= 0)
	{
		cout << "failed to search the workpiece" << endl;
		return;
	}

	sort(contours.begin(), contours.end(), compareArea);

	vector<Point> Contour;
	Contour = *(contours.begin());		//ѡ����������ڶ����������Ϊ��������

	imgContour = image.clone();			//��Դͼ����и���
	vector<vector<Point>> vectemp;				//�����������
	vectemp.clear();
	vectemp.push_back(Contour);
	drawContours(imgContour, vectemp, -1, RED);

	//imgContour = image.clone();			//��Դͼ����и���
	//drawContours(imgContour, contours, -1, RED);

	imshow(dstwin, imgDst);
	imshow(contourwin, imgContour);
	imwrite("contours.jpg", imgContour);
}