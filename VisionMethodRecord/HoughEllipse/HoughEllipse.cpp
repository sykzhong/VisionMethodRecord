#include "stdafx.h"
#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <map>

#include <Eigen/Dense>
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  


#include "easylogging++.h"
using namespace std;
using namespace cv;
using namespace Eigen;
using namespace el;


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
	dstPoint() : center(Point2f(0, 0)), th(0), width(0), height(0), vecPoint() {};
	vector<Point2f> vecPoint;		//用于计算的三点
	Point2f center;
	double th;
	double width, height;					//长短轴
};

struct Pointnum
{
	Pointnum() :loc(0, 0), count(0) {};
	Point2f loc;
	int count;
};

int createEllipse(Mat &srcImage, const char* filename = "")
{
	if (filename == "")
		filename = "ellipse2.jpg";
	srcImage = imread(filename, 0);
	if (srcImage.empty())
	{
		cout << "Couldn't open image " << endl;
		return 0;
	}
	vector<vector<Point> > contours;
	Mat bimage = srcImage >= 10;			//二值化

	findContours(bimage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);		//提取轮廓

	//我们将在cimage上面绘图  
	Mat cimage = Mat::zeros(bimage.size(), CV_8UC1);
	ofstream fresult;
	fresult.open("contour1.txt", ios::in | ios::trunc);

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

int Compareth(dstPoint &first, dstPoint &second)
{
	return first.th < second.th;
}

void drawellipse(dstPoint &src, Mat &dst)
{
	RotatedRect rect;
	rect.angle = src.th + 90;
	rect.center = src.center;
	rect.size.width = src.width;
	rect.size.height = src.height;
	ellipse(dst, rect, Scalar(0, 0, 255), 2);
}

INITIALIZE_EASYLOGGINGPP

int main()
{
	Configurations conf("my-conf.conf");
	Loggers::reconfigureLogger("default", conf);
	LOG(TRACE) << "Begin";

	Mat srcImage;
	createEllipse(srcImage, "test2.jpg");
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

	srand((unsigned)time(NULL));			//初始化随机函数
	const int minindex = 0;
	const int maxindex = orgPoint.size() - 1;

	///////////////////用于调节的重要参数//////////////
	const int linesize = 4;					//用于直线拟合的范围(important)
	const int maxiter = 10*orgPoint.size();	//最大迭代次数(important)
	double locres = 1;						//椭圆中心位置调节精度
	double angleres = 1;					//角度识别精度
	////////////////////////////////////////////////////

	dstPoint tmpdstpoint;					//用于暂存各三点构成的结果
	map<int, vector<dstPoint>> result;		//结果集合

	Point2f tmpcenter;
	vector<Point2f> veccenter;

	vector<Line> vecLine(orgPoint.size());

	for (int i = 0; i < orgPoint.size(); i++)
		getLine(orgPoint[i], linesize, vecLine[i]);

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
			tmpdstpoint.vecPoint.clear();
			xindex = tmpcenter.x / locres;
			yindex = tmpcenter.y / locres;
			tmpdstpoint.vecPoint.push_back(p1);
			tmpdstpoint.vecPoint.push_back(p2);
			tmpdstpoint.vecPoint.push_back(p3);
			tmpdstpoint.center = tmpcenter;
			result[yindex*Image.cols + xindex].push_back(tmpdstpoint);
		}
		veccenter.push_back(tmpcenter);
	}

	int locindex;
	int maxlocsize = 0;
	map<int, vector<dstPoint>>::iterator iter;
	for (iter = result.begin(); iter != result.end(); iter++)
		if ((*iter).second.size() > maxlocsize)
		{
			maxlocsize = (*iter).second.size();
			locindex = iter->first;
		}

	VectorXd A(3), B(3);			//A存储了椭圆标准方程的ABC
	MatrixXd X(3, 3);
	B << 1, 1, 1;

	double xprime[3], yprime[3];


	for (int i = 0; i < result[locindex].size(); i++)
	{
		dstPoint &tmpdstpoint = result[locindex][i];
		double &th = tmpdstpoint.th;
		for (int j = 0; j < 3; j++)
		{
			xprime[j] = tmpdstpoint.vecPoint[j].x - tmpdstpoint.center.x;
			yprime[j] = tmpdstpoint.vecPoint[j].y - tmpdstpoint.center.y;
		}

		X << xprime[0] * xprime[0], 2 * xprime[0] * yprime[0], yprime[0] * yprime[0],
			xprime[1] * xprime[1], 2 * xprime[1] * yprime[1], yprime[1] * yprime[1],
			xprime[2] * xprime[2], 2 * xprime[2] * yprime[2], yprime[2] * yprime[2];

		A = X.inverse()*B;

		if (_isnan(A[0]) || _isnan(A[1]) || _isnan(A[2]))			//当X无法求逆时，跳过该结果
		{
			result[locindex].erase(result[locindex].begin() + i);
			i--;
			continue;
		}

		if (A[1] == 0 && A[0] <= A[2])
			th = 0;
		else if (A[1] == 0 && A[0] > A[2])
			th = PI / 2;
		else
			th = atan((A[2] - A[0] - sqrt((A[0] - A[2])*(A[0] - A[2]) + 4 * A[1] * A[1])) / (2*A[1]));

		VectorXd C(2), D(2);			//Y*D = C
		MatrixXd Y(2, 2);

		C << A[0], A[2];
		Y << cos(th)*cos(th), sin(th)*sin(th),
			sin(th)*sin(th), cos(th)*cos(th);


		D = Y.inverse()*C;

		tmpdstpoint.th = tmpdstpoint.th / PI * 180;
		tmpdstpoint.width = 2*min(sqrt(1 / D[0]), sqrt(1 / D[1]));
		tmpdstpoint.height = 2*max(sqrt(1 / D[0]), sqrt(1 / D[1]));
		
		//LOG(TRACE) << "\nX = \n" << X;
		//LOG(TRACE) << "\nX^-1 = \n" << X.inverse();
		//LOG(TRACE) << "_isnan(width) = " << _isnan(tmpdstpoint.width);
		//LOG(TRACE) << "\nD = \n" << D;
		//LOG(TRACE) << "th = " << th;
		//LOG(TRACE) << "width = " << tmpdstpoint.width << "\n";

	}

	int tmpmaxsize = 0;
	int tmpindex = 0;

	int thindex = 0;
	int maxthsize = 0;

	dstPoint meanresult;
	dstPoint tmpresult;

	vector<dstPoint> &vecdstpoint = result[locindex];
	sort(vecdstpoint.begin(), vecdstpoint.end(), Compareth);
	for (int i = 0; i < vecdstpoint.size(); i++)
	{
		if (vecdstpoint[i].th <= vecdstpoint[tmpindex].th + angleres)
		{
			tmpmaxsize++;

			tmpresult.center += vecdstpoint[i].center;
			tmpresult.th += vecdstpoint[i].th;
			tmpresult.width += vecdstpoint[i].width;
			tmpresult.height += vecdstpoint[i].height;
		}
		else
		{
			if (tmpmaxsize > maxthsize)
			{
				maxthsize = tmpmaxsize;
				thindex = tmpindex;
				
				meanresult.center = Point2f(tmpresult.center.x / maxthsize, tmpresult.center.y / maxthsize);
				meanresult.th = tmpresult.th / maxthsize;
				meanresult.width = tmpresult.width / maxthsize;
				meanresult.height = tmpresult.height / maxthsize;

			}

			tmpresult.center -= vecdstpoint[tmpindex].center;
			tmpresult.th -= vecdstpoint[tmpindex].th;
			tmpresult.width -= vecdstpoint[tmpindex].width;
			tmpresult.height -= vecdstpoint[tmpindex].height;

			tmpindex++;
			tmpmaxsize--;
			i--;
		}
	}
	cvtColor(srcImage, srcImage, CV_GRAY2BGR);
	drawellipse(meanresult, srcImage);
	imshow("result", srcImage);
	//cvtColor(Image, Image, CV_GRAY2BGR);
	//drawellipse(meanresult, Image);
	//imshow("result", Image);
	waitKey(0);
}