// HoughTrans.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <map>
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  
using namespace std;
using namespace cv;

const double PI = 3.141592654;

vector<Point> vecPoint;
Mat image;

struct node
{
	//标准方程:Ax^2 + 2Bxy + Cy^2 + 2Dx + 2Ey + F = 0
	//圆心:(u, v)
	//u = (CD - BE) / (B^2 - AC);
	//v = (AE - BD) / (B^2 - AC);
	//椭圆旋转角度:th
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

int getLine(Point &_P, const int &_size, double &_a, double &_b, double &_c, double &_th)
{
	int xstart = max(0, _P.x - _size);
	int xend = min(image.cols, _P.x + _size);
	int ystart = max(0, _P.y - _size);
	int yend = min(image.rows, _P.y + _size);
	vector<Point> tmpvecPoint;
	for (int i = xstart; i < xend; i++)
		for (int j = ystart; j < yend; j++)
			if (image.at<uchar>(j, i) != 0)
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
		Dxy += fabs((tmpvecPoint[i].x - x_mean) * (tmpvecPoint[i].y - y_mean));
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
	
	return true;
}

void checkPoint(int _x, int _y)
{
	
}

int main()
{
	freopen("contour.txt", "r", stdin);
	Point tmppoint;
	string strinput;
	while (getline(cin, strinput))
	{
		if (strinput == "")
			break;
		sscanf(strinput.c_str(), "%d %d", &tmppoint.x, &tmppoint.y);
		vecPoint.push_back(tmppoint);
	}
	image = imread("image.jpg", 0);
	if (!image.data)
		return 0;
#ifdef TEST
	imshow("", image);
	waitKey(0);
	return 0;
#else
	srand((unsigned)time(NULL));		//初始化随机函数
	const int minindex = 0;
	const int maxindex = vecPoint.size() - 1;

	const int maxiter = vecPoint.size() / 2;				//最大迭代次数
	const int border = 5;				//l3判断边界角度值
	int index1, index2;					//l1, l2经过的几点在vecPoint中的位置
	const int linesize = 2;				//直线拟合领域大小
	double a1, b1, c1, a2, b2, c2, a3, b3, c3;			//分别记录l1, l2, l3参数
	double th1, th2, th3;
	double an, bn, cn, thn;					//记录P3切线参数

	vector<rnode> result;
	vector<double> testresult;
	node tmpnode;

	Point M, T, G;						//M表示P1, P2中点， T表示l1, l2交点, G表示M, T中点
	for (int i = 0; i < maxiter; i++)
	{
		index1 = (rand() % (maxindex - minindex + 1)) + minindex;
		index2 = (rand() % (maxindex - minindex + 1)) + minindex;
		if (index1 == index2)
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

		double amt, bmt, cmt;		//直线MT的参数
		amt = (double)M.y - T.y;
		bmt = (double)T.x - M.x;
		cmt = (double)T.y*(M.x - T.x) -
			(double)T.x*(M.y - T.y);
		

		vector<Point> tobecheck;			//存储MT之间的点
		double tmpx, tmpy;
		int tmpxmax = min(image.cols, max(M.x, G.x));
		int tmpymax = min(image.rows, max(M.y, G.y));
		for (int j = max(0, min(M.x, G.x)); j < tmpxmax; j++)
		{
			if (bmt == 0)
			{
				for (int k = max(0, min(M.y, G.y)); k < tmpymax; k++)
				{
					tmpx = M.x;
					tmpy = k;
					tobecheck.push_back(Point(tmpx, tmpy));
				}
			}
			else
			{
				tmpx = j;
				tmpy = (-amt / bmt)*j - cmt / bmt;
				if (tmpy < 0)
				{
					if (amt != 0)
					{
						j = -cmt / amt;			//找出tmpy = 0时的x坐标
						continue;
					}
					else
						break;						//直接跳出循环
				}
				if (tmpy >= tmpymax)
					break;
				tobecheck.push_back(Point(tmpx, tmpy));
			}
		}


		for (int j = 0; j < tobecheck.size(); j++)
		{
			tmpx = tobecheck[j].x;
			tmpy = tobecheck[j].y;
			if (image.at<uchar>(tmpy, tmpx) != 0)
			{
				getLine(Point(tmpx, tmpy), linesize, an, bn, cn, thn);
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
						rnode tmprnode;
						tmprnode.u = (tmpnode.C*tmpnode.D - tmpnode.B*tmpnode.E) / (tmpnode.B*tmpnode.B - tmpnode.A*tmpnode.C);
						tmprnode.v = (tmpnode.A*tmpnode.E - tmpnode.B*tmpnode.D) / (tmpnode.B*tmpnode.B - tmpnode.A*tmpnode.C);
						testresult.push_back(tmprnode.u);
						//tan(2*th) = 2B / (A - C)
						//if(A == C) th = +-PI/4
					}
				}
			}
		}
	}
	sort(testresult.begin(), testresult.end());
    return 0;
#endif
}

