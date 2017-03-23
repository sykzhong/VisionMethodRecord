// HoughTrans.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  
using namespace std;
using namespace cv;

const double PI = 3.141592654;

vector<Point2f> vecPoint;
Mat image;

struct node
{
	//��׼����:Ax^2 + 2Bxy + Cy^2 + 2Dx + 2Ey + F = 0
	//Բ��:(u, v)
	//u = (CD - BE) / (B^2 - AC);
	//v = (AE - BD) / (B^2 - AC);
	//��Բ��ת�Ƕ�:th
	//tan(2*th) = 2B / (A - C)
	//if(A == C) th = +-PI/4
	double A;
	double B;
	double C;
	double D;
	double E;
	double F;
};

struct rnode
{ 
	double u;
	double v;
	double th;
};

int getImage()
{
	const char* filename = "ellipse2.jpg";
	//��ȡͼ��  
	Mat src = imread(filename, 0);
	if (src.empty())
	{
		cout << "Couldn't open image " << endl;
		return 0;
	}
	vector<vector<Point> > contours;
	//����൱�ڶ�ֵ�������matlab���Ǿ���� Iwt = Iw>=threshold;  
	Mat bimage = src >= 10;
	//Mat bimage;  
	//threshold(image, bimage, sliderPos, 255,CV_THRESH_BINARY);  

	//��ȡ����,�൱��matlab����ͨ�������  
	findContours(bimage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	//���ǽ���cimage�����ͼ  
	Mat cimage = Mat::zeros(bimage.size(), CV_8UC1);
	ofstream fresult;
	fresult.open("contour1.txt", ios::in);

	for (size_t i = 0; i < contours.size(); i++)
	{
		//�����ı�Ե�����  
		size_t count = contours[i].size();
		//Fitzgibbon����Բ��Ϸ�����Ҫ������6���㣬���ף�Direct Least Squares Fitting of Ellipses[1999]  
		if (count < 6)
			continue;

		if (fresult.is_open())
		{
			for (size_t j = 0; j < contours[i].size(); j++)
			{
				fresult << contours[i][j].x << " " << contours[i][j].y << endl;
			}
		}
		drawContours(cimage, contours, (int)i, Scalar::all(255), 1, 8);
		Mat pointsf;
		//�������еĵ�ת��Ϊ��Mat��ʽ�洢��2ά�㼯(x,y)  
		Mat(contours[i]).convertTo(pointsf, CV_32F);

		//��С������ϣ�Fitzgibbon�ķ�����  
		//box��������Բ��5��������(x,y,w,h,theta)  
		RotatedRect box = fitEllipse(pointsf);

		//����Щ���������֮�Ⱥܶ����Щ��Բ�޳���  
		if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 8)
			continue;
		//��������  
		
	}

	
	imwrite("image1.jpg", cimage);

}

#define OPENCVLINE

#ifndef OPENCVLINE
int getLine(Point2f &_P, const int &_size, double &_a, double &_b, double &_c, double &_th)
{
	int xstart = max((float)0, _P.x - _size);
	int xend = min((float)image.cols - 1, _P.x + _size);
	int ystart = max((float)0, _P.y - _size);
	int yend = min((float)image.rows - 1, _P.y + _size);
	vector<Point2f> tmpvecPoint;
	for (int i = xstart; i <= xend; i++)
		for (int j = ystart; j <= yend; j++)
			if (image.at<uchar>(j, i) > 10)
				tmpvecPoint.push_back(Point(i, j));

	if (tmpvecPoint.size() < 2)
	{
		_a = 0;
		_b = 0;
		_c = 0;
		_th = 0;
		return false;
	}
	double x_mean = 0;
	double y_mean = 0;

	for (int i = 0; i < tmpvecPoint.size(); i++)
	{
		x_mean += tmpvecPoint[i].x;
		y_mean += tmpvecPoint[i].y;
	}
	x_mean /= tmpvecPoint.size();
	y_mean /= tmpvecPoint.size();

	double Dxx = 0, Dxy = 0, Dyy = 0;

	for (int i = 0; i < tmpvecPoint.size(); i++)
	{
		Dxx += (tmpvecPoint[i].x - x_mean) * (tmpvecPoint[i].x - x_mean);
		Dxy += (tmpvecPoint[i].x - x_mean) * (tmpvecPoint[i].y - y_mean);
		Dyy += (tmpvecPoint[i].y - y_mean) * (tmpvecPoint[i].y - y_mean);
	}
	double lambda = ((Dxx + Dyy) - sqrt((Dxx - Dyy) * (Dxx - Dyy) + 4 * Dxy * Dxy)) / 2.0;
	double den = sqrt(Dxy * Dxy + (lambda - Dxx) * (lambda - Dxx));
	if (Dxx == 0)
	{
		_a = 1;
		_b = 0;
		_c = -x_mean;
	}
	else if (Dyy == 0)
	{
		_a = 0;
		_b = 1;
		_c = -y_mean;
	}
	else
	{
		_a = Dxy / den;
		_b = (lambda - Dxx) / den;
		_c = -_a * x_mean - _b * y_mean;
	}

	if (_b == 0)
		_th = PI / 2;
	else
		_th = atan(-_a / _b);

	if (_th < 0)
		_th += PI;

	return true;
}
#else
int getLine(Point2f &_P, const int &_size, double &_a, double &_b, double &_c, double &_th)
{
	int xstart = max((float)0, _P.x - _size);
	int xend = min((float)image.cols - 1, _P.x + _size);
	int ystart = max((float)0, _P.y - _size);
	int yend = min((float)image.rows - 1, _P.y + _size);
	vector<Point2f> tmpvecPoint;
	for (int i = xstart; i <= xend; i++)
		for (int j = ystart; j <= yend; j++)
			if (image.at<uchar>(j, i) > 10)
				tmpvecPoint.push_back(Point(i, j));

	Vec4f line;
	fitLine(tmpvecPoint, line, CV_DIST_HUBER, 0, 0.01, 0.01);
	_a = -line[1];
	_b = line[0];
	double x0 = line[2], y0 = line[3];
	_c = -(_a*x0 + _b*y0);
	if (_b == 0)
		_th = PI / 2;
	else
		_th = atan(-_a / _b);

	if (_th < 0)
		_th += PI;


	return true;
}
#endif

double getDist(Point2f first, Point2f second)
{
	Point2f tmp = first - second;

	return sqrt(tmp.x*tmp.x + tmp.y*tmp.y);
}

int Comparex(Point2f first, Point2f second)
{
	return first.x < second.x;
}

int main()
{
	getImage();
	freopen("contour1.txt", "r", stdin);
	Point2f tmppoint;
	string strinput;
	while (getline(cin, strinput))
	{
		if (strinput == "")
			break;
		sscanf(strinput.c_str(), "%f %f", &tmppoint.x, &tmppoint.y);
		vecPoint.push_back(tmppoint);
	}
	image = imread("image1.jpg", 0);
	if (!image.data)
		return 0;

	srand((unsigned)time(NULL));		//��ʼ���������
	const int minindex = 0;
	const int maxindex = vecPoint.size() - 1;

	const int maxiter = vecPoint.size();				//����������
	const int border = 20;				//l3�жϱ߽�Ƕ�ֵ
	int index1, index2;					//l1, l2�����ļ�����vecPoint�е�λ��
	const int linesize = 5;				//ֱ����������С
	double a1, b1, c1, a2, b2, c2, a3, b3, c3;			//�ֱ��¼l1, l2, l3����
	double th1, th2, th3;
	double an, bn, cn, thn;					//��¼P3���߲���

	vector<rnode> result;
	vector<Point> testresult;
	node tmpnode;

	Point2f M, T, G;						//M��ʾP1, P2�е㣬 T��ʾl1, l2����, G��ʾM, T�е�
	for (int i = 0; i < maxiter; i++)
	{
		index1 = (rand() % (maxindex - minindex + 1)) + minindex;
		index2 = (rand() % (maxindex - minindex + 1)) + minindex;
		if (index1 == index2 || 
			getDist(vecPoint[index1], vecPoint[index2]) < 30)
		{
			i--;
			continue;
		}
		getLine(vecPoint[index1], linesize, a1, b1, c1, th1);
		getLine(vecPoint[index2], linesize, a2, b2, c2, th2);
		a3 = (double)vecPoint[index1].y - vecPoint[index2].y;
		b3 = (double)vecPoint[index2].x - vecPoint[index1].x;
		c3 = (double)vecPoint[index2].y*(vecPoint[index1].x - vecPoint[index2].x) -
			(double)vecPoint[index2].x*(vecPoint[index1].y - vecPoint[index2].y);

		if (b3 == 0)
			th3 = PI / 2;
		else
			th3 = atan(-a3 / b3);
		if (th3 < 0)
			th3 += PI;
		if (fabs(th1 - th2) <= (double)1 / 180 * PI)
		{
			i--;
			continue;
		}

		M.x = (vecPoint[index1].x + vecPoint[index2].x) / 2;
		M.y = (vecPoint[index1].y + vecPoint[index2].y) / 2;

		T.x = -(c1*b2 - c2*b1) / (a1*b2 - a2*b1);
		T.y = -(c1*a2 - c2*a1) / (b1*a2 - b2*a1);

		G.x = (M.x + T.x) / 2;
		G.y = (M.y + T.y) / 2;

		double amt, bmt, cmt;		//ֱ��MT�Ĳ���
		amt = (double)M.y - T.y;
		bmt = (double)T.x - M.x;
		cmt = (double)T.y*(M.x - T.x) -
			(double)T.x*(M.y - T.y);
		

		vector<Point2f> tobecheck;			//�洢MT֮��ĵ�
		double tmpx, tmpy;
		int tmpxmax = min((float)image.cols - 1, max(M.x, G.x));
		int tmpymax = min((float)image.rows - 1, max(M.y, G.y));
		for (int j = max((float)0, min(M.x, G.x)); j <= tmpxmax; j++)
		{
			if (bmt == 0)
			{
				for (int k = max((float)0, min(M.y, G.y)); k <= tmpymax; k++)
				{
					tmpx = M.x;
					tmpy = k;
					tobecheck.push_back(Point2f(tmpx, tmpy));
				}
			}
			else
			{
				tmpx = j;
				tmpy = (-amt / bmt)*j - cmt / bmt;
				if ((tmpy > tmpymax && (-amt / bmt) > 0) || (tmpy < 0 && (-amt / bmt) < 0))
					break;
				else if (tmpy > tmpymax || tmpy < 0)
					continue;
				tobecheck.push_back(Point2f(tmpx, tmpy));
			}
		}


		for (int j = 0; j < tobecheck.size(); j++)
		{
			tmpx = tobecheck[j].x;
			tmpy = tobecheck[j].y;
			if (image.at<uchar>(tmpy, tmpx) > 10)
			{
				getLine(Point2f(tmpx, tmpy), linesize, an, bn, cn, thn);
				if (fabs(thn - th3) <= (double)border / 180 * PI)
				{
					double lambda = -(a1*a2*tmpx*tmpx + (a1*b2 + a2*b1)*tmpx*tmpy + b1*b2*tmpy*tmpy
						+ (a1*c2 + a2*c1)*tmpx + (b1*c2 + b2*c1)*tmpy + c1*c2) /
						(a3*a3*tmpx*tmpx + 2*a3*b3*tmpx*tmpy + b3*b3*tmpy*tmpy + 2*a3*c3*tmpx + 2*b3*c3*tmpy + c3*c3);
					tmpnode.A = a1*a2 + lambda*a3*a3;
					tmpnode.B = ((a1*b2 + a2*b1) + 2*lambda*a3*b3) / 2;
					tmpnode.C = b1*b2 + lambda*b3*b3;
					tmpnode.D = (a1*c2 + a2*c1 + 2*lambda*a3*c3) / 2;
					tmpnode.E = (b1*c2 + b2*c1 + 2*lambda*b3*c3) / 2;
					tmpnode.F = c1*c2 + lambda*c3*c3;
					if (tmpnode.B*tmpnode.B - tmpnode.A*tmpnode.C < 0)
					{
						//Mat tmpresult = image.clone();
						//line(tmpresult, vecPoint[index1], T, Scalar::all(255));
						//line(tmpresult, vecPoint[index2], T, Scalar::all(255));
						//line(tmpresult, vecPoint[index1], vecPoint[index2], Scalar::all(255));
						//line(tmpresult, M, T, Scalar::all(255));
						//if (bn != 0)
						//{
						//	Point2f tmppoint = Point2f(tmpx + 100, (-an / bn)*(tmpx + 100) - cn / bn);
						//	line(tmpresult, Point2f(tmpx, tmpy), tmppoint, Scalar::all(255));
						//}
						//
						
						
						
						Point tmprnode;
						tmprnode.x = cvRound((tmpnode.C*tmpnode.D - tmpnode.B*tmpnode.E) / (tmpnode.B*tmpnode.B - tmpnode.A*tmpnode.C));
						tmprnode.y = cvRound((tmpnode.A*tmpnode.E - tmpnode.B*tmpnode.D) / (tmpnode.B*tmpnode.B - tmpnode.A*tmpnode.C));
						if (tmprnode.x < 0 || tmprnode.y < 0 || tmprnode.x > image.cols || tmprnode.y > image.rows)
							continue;
						else
							testresult.push_back(tmprnode);

						//Point2f tmpcenter = Point2f(tmprnode.x, tmprnode.y);

						//circle(tmpresult, tmpcenter, 3, Scalar::all(255), -1);
						//line(tmpresult, tmpcenter, T, Scalar::all(255));

						////Point2f realcenter = Point2f(470, 330);
						//Point2f realcenter = Point2f(528, 351);
						//circle(tmpresult, realcenter, 2, Scalar::all(255), -1);
						//line(tmpresult, realcenter, T, Scalar::all(255));

						//circle(tmpresult, M, 2, Scalar::all(255), -1);
						//imwrite("tmpresult.jpg", tmpresult);
					}
				}
			}
		}
	}
	sort(testresult.begin(), testresult.end(), Comparex);
	vector<int> vecHalc(image.cols, 0);			//��1��Ϊ�߽�
	float start = testresult.begin()->x;
	int tmpindex1 = 0, tmpindex2 = 0;
	int tmpcount = 0;
	for (tmpindex1 = 0; tmpindex1 < testresult.size(); tmpindex1++)
	{
		if (testresult[tmpindex1].x <= start + 1)
			tmpcount++;
		else
		{
			vecHalc[start] = tmpcount;
			tmpcount = 0;
			start++;
			tmpindex1--;
		}
	}

	Mat tmpimg = Mat(image.size(), CV_8UC1, Scalar::all(0));
	float x_mean = 0, y_mean = 0;
	for (int i = 0; i < testresult.size(); i++)
	{
		circle(tmpimg, testresult[i], 2, Scalar::all(255), -1);
		x_mean += testresult[i].x;
		y_mean += testresult[i].y;
	}
	x_mean /= testresult.size();
	y_mean /= testresult.size();
	cout << x_mean << endl;
	cout << y_mean << endl;
	imshow("result", tmpimg);
	waitKey(0);
	//sort(testresult.begin(), testresult.end());
    return 0;
}

