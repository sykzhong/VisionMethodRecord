#include "stdafx.h"
#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>

#include <Eigen/Dense>
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  
using namespace std;
using namespace cv;
using namespace Eigen;

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

struct dstPoint
{
	vector<Point2f> vecPoint;		//用于计算的三点
	Point2f center;
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
		if (count < 6 || (i > 0 && contours[i].size() < contours[i-1].size()))
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
	createEllipse("ellipse3.jpg");
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
	int index1, index2, index3;	//三点的坐标
	Point2f p12, p23;			//三切线交点

	const int linesize = 6;		//用于直线拟合的范围(important)

	srand((unsigned)time(NULL));			//初始化随机函数
	const int minindex = 0;
	const int maxindex = orgPoint.size() - 1;

	const int maxiter = orgPoint.size();	//最大迭代次数

	float res = 5;		//判断精度
	int xnum = Image.cols / res;
	int ynum = Image.rows / res;
	vector<vector<vector<dstPoint>>> result(xnum, vector<vector<dstPoint>>(ynum));		//用于存储判断点，二维数组行表示x，列表示y
	dstPoint tmpdstpoint;

	Point2f tmpcenter;
	vector<Point2f> veccenter;

	vector<Line> vecLine(orgPoint.size());
	//int debugcount = 0;
	for (int i = 0; i < orgPoint.size(); i++)
	{
		//cout << debugcount++ << endl;
		//if(i == 1400)
		getLine(orgPoint[i], linesize, vecLine[i]);
	}

	for (int i = 0; i < maxiter; i++)
	{
		index1 = (rand() % (maxindex - minindex + 1)) + minindex;
		index2 = (rand() % (maxindex - minindex + 1)) + minindex;
		index3 = (rand() % (maxindex - minindex + 1)) + minindex;
		l1 = vecLine[index1];
		l2 = vecLine[index2];
		l3 = vecLine[index3];
		if (fabs(l1.th - l2.th) <= (double)5 / 180 * PI || fabs(l2.th - l3.th) <= (double)5 / 180 * PI)
			continue;

		p1 = orgPoint[index1];
		p2 = orgPoint[index2];
		p3 = orgPoint[index3];
		if (getDist(p1, p2) < 50 || getDist(p2, p3) < 50)
			continue;

		getIntersection(l1, l2, p12);
		getIntersection(l2, l3, p23);

		getLine(p12, (p1 + p2) / 2, l12);
		getLine(p23, (p2 + p3) / 2, l23);

		getIntersection(l12, l23, tmpcenter);
		if (tmpcenter.x < 0 || tmpcenter.y < 0 || tmpcenter.x > Image.cols || tmpcenter.y > Image.rows)
			continue;
		else
		{
			int xindex, yindex;
			
			xindex = tmpcenter.x / res;
			yindex = tmpcenter.y / res;
			tmpdstpoint.vecPoint.push_back(p1);
			tmpdstpoint.vecPoint.push_back(p2);
			tmpdstpoint.vecPoint.push_back(p3);
			tmpdstpoint.center = tmpcenter;
			result[xindex][yindex].push_back(tmpdstpoint);
		}
		veccenter.push_back(tmpcenter);
	}
	int tmpxindex = 0;
	int tmpyindex = 0;
	int tmpmaxsize = 0;
	for(int i = 0; i < xnum; i++)
		for (int j = 0; j < ynum; j++)
		{
			if (result[i][j].size() > tmpmaxsize)
			{
				tmpmaxsize = result[i][j].size();
				tmpxindex = i;
				tmpyindex = j;
			}
		}
	
	VectorXf A(3), B(3);
	MatrixXf X(3, 3);
	B << 1, 1, 1;

	float xprime[3], yprime[3];
	float th;
	vector<float> vecth;
	for (int i = 0; i < result[tmpxindex][tmpyindex].size(); i++)
	{
		tmpdstpoint = result[tmpxindex][tmpyindex][i];
		for (int j = 0; j < 3; j++)
		{
			xprime[j] = tmpdstpoint.vecPoint[j].x - tmpdstpoint.center.x;
			yprime[j] = tmpdstpoint.vecPoint[j].y - tmpdstpoint.center.y;
		}

		X << xprime[0] * xprime[0], 2 * xprime[0] * yprime[0], yprime[0] * yprime[0],
			xprime[1] * xprime[1], 2 * xprime[1] * yprime[1], yprime[1] * yprime[1],
			xprime[2] * xprime[2], 2 * xprime[2] * yprime[2], yprime[2] * yprime[2];

		A = X.inverse()*B;
		
		/*if (A[0] != A[2])
		{
			th = atan(2 * A[1] / (A[0] - A[2]));
			th /= 2;
		}
		else th = PI / 4;*/

		if (A[1] == 0 && A[0] <= A[2])
			th = 0;
		else if (A[1] == 0 && A[0] > A[2])
			th = PI / 2;
		else
			th = atan((A[2] - A[0] - sqrt((A[0] - A[2])*(A[0] - A[2]) + A[1] * A[1])) / A[1]);

		th = th / PI * 180;

		vecth.push_back(th);

		//cout << A << endl;
		//cout << th / PI * 180 << endl;
		//cout << endl;
	}
	sort(vecth.begin(), vecth.end());

	//vector<int> xHalc(Image.cols / res, 0);
	//vector<int> yHalc(Image.cols / res, 0);

	//for (int i = 0; i < veccenter.size(); i++)
	//{
	//	int xindex, yindex;
	//	xindex = veccenter[i].x / res;
	//	yindex = veccenter[i].y / res;
	//	xHalc[xindex]++;
	//	yHalc[yindex]++;
	//}

	//Point2f finalcenter;
	//finalcenter.x = (max_element(xHalc.begin(), xHalc.end(), Comparei) - xHalc.begin()) * res;
	//finalcenter.y = (max_element(yHalc.begin(), yHalc.end(), Comparei) - yHalc.begin()) * res;

	
}