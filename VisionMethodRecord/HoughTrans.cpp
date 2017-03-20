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
	//tan(2th) = 2B / (A - C)
	//if(A == C) th = +-PI/4
	double A;
	double B;
	double C;
	double D;
	double E;
	double F;
};

int getLine(Point &_P, const int &_size, double &_a, double &_b, double &_c, double &_th)
{
	int xstart = max(0, _P.x - _size);
	int xend = min(image.cols, _P.x + _size);
	int ystart = max(0, _P.y - _size);
	int yend = min(image.rows, _P.y + _size);
	vector<Point> tmpvecPoint;
	for (int i = xstart; i < xend; i++)
		for (int j = ystart; j < ystart; j++)
			if (image.at<float>(j, i) != 0)
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

	for (int i = 0; i <= tmpvecPoint.size(); i++)
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
	_a = Dxy / den;
	_b = (lambda - Dxx) / den;
	_c = -_a * x_mean - _b * y_mean;
	_th = atan2(-_a, _b);
	return true;
}

int main()
{
	freopen("vecPoint.txt", "r", stdin);
	Point tmppoint;
	string strinput;
	while (getline(cin, strinput))
	{
		if (strinput == "")
			break;
		sscanf(strinput.c_str(), "%d %d", tmppoint.x, tmppoint.y);
		vecPoint.push_back(tmppoint);
	}
	image = imread("test.jpg", 0);
	srand((unsigned)time(NULL));		//初始化随机函数
	const int minindex = 0;
	const int maxindex = vecPoint.size() - 1;

	const int maxiter = 30;				//最大迭代次数
	const int border = 20;				//l3判断边界角度值
	int index1, index2;					//l1, l2经过的几点在vecPoint中的位置
	const int linesize = 2;				//直线拟合领域大小
	double a1, b1, c1, a2, b2, c2, a3, b3, c3;			//分别记录l1, l2, l3参数
	double th1, th2, th3;
	double an, bn, cn;					//记录P3切线参数

	map<node, int> result;				//用于记录Node结果出现次数
	node tmpnode;

	Point M, T, G;						//M表示P1, P2中点， T表示l1, l2交点, G表示M, T中点
	for (int i = 0; i < maxiter; i++)
	{
		index1 = (rand() % (maxindex - minindex + 1)) + minindex;
		index2 = (rand() % (maxindex - minindex + 1)) + minindex;
		getLine(vecPoint[index1], linesize, a1, b1, c1, th1);
		getLine(vecPoint[index2], linesize, a2, b2, c2, th2);
		if (vecPoint[index1].x != vecPoint[index2].x)
		{
			a3 = (vecPoint[index2].y - vecPoint[index1].y) / (vecPoint[index1].x - vecPoint[index2].x);
			b3 = 0;
			c3 = -(vecPoint[index2].y*vecPoint[index1].x - vecPoint[index2].x*vecPoint[index1].y) / 
				(vecPoint[index1].x - vecPoint[index2].x);
		}
		else
		{
			a3 = 0;
			b3 = (vecPoint[index1].x - vecPoint[index2].x) / (vecPoint[index2].y - vecPoint[index1].y);
			c3 = -(vecPoint[index1].x*vecPoint[index2].y - vecPoint[index2].x*vecPoint[index1].y) / 
				(vecPoint[index2].y - vecPoint[index1].y);
		}
		if (fabs(th1 - th2) <= (double)1 / 180 * PI)
		{
			i--;
			continue;
		}
		M.x = (vecPoint[index1].x + vecPoint[index2].x) / 2;
		M.y = (vecPoint[index1].y + vecPoint[index2].y) / 2;

		T.x = -(c1*b2 - c2*b1) / (a1*b2 - a2*b1);
		T.y = -(c1*a2 - c2*a1) / (b1*a2 - b2*a2);

		G.x = (M.x + T.x) / 2;
		G.y = (M.y + M.y) / 2;

		double tmpk = (M.y - T.y) / (M.x - T.x);
		double tmpb = (T.y*M.x - T.x*M.y) / (M.x - T.x);

		for (int j = min(M.x, G.x); j < max(M.x, G.x); j++)
		{
			
			if (image.at<float>(j*tmpk + tmpb, j) != 0)
				getLine(Point(j, j*tmpk + tmpb), linesize, a3, b3, c3, th3);
			if (th3 <= (double)border / 180 * PI)
			{
				int x3 = j, y3 = j*tmpk + tmpb;
				double lambda = -(a1*a2*x3*x3 + (a1*b2 + a2*b1)*x3*y3 + b1*b2*y3*y3
					+ (a1*c2 + a2*c1)*x3 + (b1*c2 + b2*c1)*x3*y3 + c1*c2) /
					(a3*x3*x3 + a3*b3*x3*y3 + b3*y3*y3 + 2 * a3*b3*x3 + 2 * b3*c3*y3 + c3*c3);
				tmpnode.A = a1*a2 + lambda*a3;
				tmpnode.B = ((a1*b2 + a2*b1) + lambda*a3*b3) / 2;
				tmpnode.C = b1*b2 + lambda*b3;
				tmpnode.D = (a1*c2 + a2*c1 + 2 * lambda*a3*c3) / 2;
				tmpnode.E = (b1*c2 + b2*c1 + 2 * lambda*b3*c3) / 2;
				tmpnode.F = c1*c2 + lambda*c3*c3;
				if (tmpnode.B*tmpnode.B - tmpnode.A*tmpnode.C < 0)
				{
					if (result.find(tmpnode) == result.end())
						result[tmpnode] = 0;
					else
						result[tmpnode]++;
				}
			}
		}
	}

	
	
    return 0;
}

