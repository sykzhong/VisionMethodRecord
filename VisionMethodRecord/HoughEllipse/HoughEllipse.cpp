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
Mat Image;
vector<Point2f> orgPoint;

struct Line
{
	//用于存储直线标准方程参数：ax+by+c = 0
	double a;
	double b;
	double c;
	double th;
};

struct Pointnum
{
	Pointnum() :loc(0, 0), count(0) {};
	Point2f loc;
	int count;
};

int createEllipse(const char* filename = "")
{
	if (filename == "")
		filename = "ellipse2.jpg";
	Mat src = imread(filename, 0);
	if (src.empty())
	{
		cout << "Couldn't open image " << endl;
		return 0;
	}
	vector<vector<Point> > contours;
	Mat bimage = src >= 10;			//二值化

	findContours(bimage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);		//提取轮廓

	//我们将在cimage上面绘图  
	Mat cimage = Mat::zeros(bimage.size(), CV_8UC1);
	ofstream fresult;
	fresult.open("contour1.txt", ios::in);

	for (size_t i = 0; i < contours.size(); i++)
	{
		size_t count = contours[i].size();
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
		Mat(contours[i]).convertTo(pointsf, CV_32F);

		RotatedRect box = fitEllipse(pointsf);		//椭圆拟合
	}
	imwrite("image1.jpg", cimage);
}

inline int getIntersection(Line &_l1, Line &_l2, Point2f &_dst)		//获取两直线交点
{
	if (_l1.a*_l2.b == _l2.a*_l1.b)
		return 0;
	_dst.x = -(_l1.c*_l2.b - _l2.c*_l1.b) / (_l1.a*_l2.b - _l2.a*_l1.b);
	_dst.y = -(_l1.c*_l2.a - _l2.c*_l1.a) / (_l1.b*_l2.a - _l2.b*_l1.a);
}

inline void getLine(Point2f &_P1, Point2f &_P2, Line &_dst)			//获取过两点的直线标准方程
{
	_dst.a = (double)_P1.y - _P2.y;
	_dst.b = (double)_P2.x - _P1.x;
	_dst.c = (double)_P2.y*(_P1.x - _P2.x) - (double)_P2.x*(_P1.y - _P2.y);
}

inline void getLine(Point2f &_P, const int &_size, Line &_l)		//获取过点的切线
{
	int xstart = max((float)0, _P.x - _size);
	int xend = min((float)Image.cols - 1, _P.x + _size);
	int ystart = max((float)0, _P.y - _size);
	int yend = min((float)Image.rows - 1, _P.y + _size);
	vector<Point2f> tmpvecPoint;
	for (int i = xstart; i <= xend; i++)
		for (int j = ystart; j <= yend; j++)
			if (Image.at<uchar>(j, i) > 10)
				tmpvecPoint.push_back(Point(i, j));

	Vec4f line;
	fitLine(tmpvecPoint, line, CV_DIST_HUBER, 0, 0.01, 0.01);
	_l.a = -line[1];
	_l.b = line[0];
	double x0 = line[2], y0 = line[3];
	_l.c = -(_l.a*x0 + _l.b*y0);

	if (_l.b == 0)
		_l.th = PI / 2;
	else
		_l.th = atan(-_l.a / _l.b);
	if (_l.th < 0)
		_l.th += PI;
}

inline double getDist(Point2f &first, Point2f &second)
{
	Point2f tmp = first - second;
	return sqrt(tmp.x*tmp.x + tmp.y*tmp.y);
}

int Comparex(Point2f &first, Point2f &second)
{
	return first.x < second.x;
}

int Comparei(int &first, int &second)
{
	return first < second;
}

int main()
{
	createEllipse();
	freopen("contour1.txt", "r", stdin);
	Point2f tmppoint;
	string strinput;
	while (getline(cin, strinput))
	{
		if (strinput == "")
			break;
		sscanf(strinput.c_str(), "%f %f", &tmppoint.x, &tmppoint.y);
		orgPoint.push_back(tmppoint);
	}
	Image = imread("image1.jpg", 0);
	if (!Image.data)
		return 0;

	Line l1, l2, l3;			//l1 l2 l3分别表示三条切线，
	Line l12, l23;				//l12 l23表示过12中点中心 23中点中心的直线

	Point2f p1, p2, p3;			//三点p1, p2, p3
	Point2f p12, p23;			//三切线交点

	const int linesize = 5;		//用于直线拟合的范围

	srand((unsigned)time(NULL));			//初始化随机函数
	const int minindex = 0;
	const int maxindex = orgPoint.size() - 1;

	const int maxiter = orgPoint.size();	//最大迭代次数

	Point2f tmpcenter;
	vector<Point2f> veccenter;

	for (int i = 0; i < maxiter; i++)
	{
		p1 = orgPoint[(rand() % (maxindex - minindex + 1)) + minindex];
		p2 = orgPoint[(rand() % (maxindex - minindex + 1)) + minindex];
		p3 = orgPoint[(rand() % (maxindex - minindex + 1)) + minindex];
		if (getDist(p1, p2) < 30 || getDist(p2, p3) < 30)
			continue;

		getLine(p1, linesize, l1);
		getLine(p2, linesize, l2);
		getLine(p3, linesize, l3);
		if (fabs(l1.th - l2.th) <= (double)5 / 180 * PI || fabs(l2.th - l3.th) <= (double)5 / 180 * PI)
			continue;

		getIntersection(l1, l2, p12);
		getIntersection(l2, l3, p23);

		getLine(p12, (p1 + p2) / 2, l12);
		getLine(p23, (p2 + p3) / 2, l23);

		getIntersection(l12, l23, tmpcenter);
		if (tmpcenter.x < 0 || tmpcenter.y < 0 || tmpcenter.x > Image.cols || tmpcenter.y > Image.rows)
			continue;
		veccenter.push_back(tmpcenter);
	}

	float res = 0.5;		//判断精度
	vector<int> xHalc(Image.cols / res, 0);
	vector<int> yHalc(Image.cols / res, 0);

	for (int i = 0; i < veccenter.size(); i++)
	{
		int xindex, yindex;
		xindex = veccenter[i].x / res;
		yindex = veccenter[i].y / res;
		xHalc[xindex]++;
		yHalc[yindex]++;
	}

	Point2f finalcenter;
	finalcenter.x = (max_element(xHalc.begin(), xHalc.end(), Comparei) - xHalc.begin()) * res;
	finalcenter.y = (max_element(yHalc.begin(), yHalc.end(), Comparei) - yHalc.begin()) * res;

	//sort(veccenter.begin(), veccenter.end(), Comparex);
	//vector<int> vecHalc(Image.cols, 0);			//以1作为边界
	//float start = veccenter.begin()->x;
	//int tmpindex = 0;
	//int tmpcount = 0;
	//for (tmpindex = 0; tmpindex < veccenter.size(); tmpindex++)
	//{
	//	if (veccenter[tmpindex].x <= start + 1)
	//		tmpcount++;
	//	else
	//	{
	//		vecHalc[start] = tmpcount;
	//		tmpcount = 0;
	//		start++;
	//		tmpindex--;
	//	}
	//}

}