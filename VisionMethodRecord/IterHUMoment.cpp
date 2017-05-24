// ContourExtraction.cpp : 定义控制台应用程序的入口点。
//阈值迭代+轮廓矩求解中心

#include "stdafx.h"
//#include "ContourExtraction.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;

const Scalar RED = Scalar(0, 0, 255);
const Scalar PINK = Scalar(230, 130, 255);
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar LIGHTBLUE = Scalar(255, 255, 160);
const Scalar GREEN = Scalar(0, 255, 0);
const Scalar BLACK = Scalar(0, 0, 0);

const int msfsp = 20;		//作为meanshift filter的像素位置差值
const int msfsr = 10;		//作为Meanshift filter的像素大小差值
const float thratio = 0.8;

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
	CommandLineParser parser(argc, argv, "{help h||}{@input|test1.jpg|}");
	parser.about("Application name v1.1");
	string filename = parser.get<string>("@input");
	if (filename.empty())
	{
		cout << "\nEmpty filename" << endl;
		return 1;
	}
	Mat image = imread(filename, 1);		//完成图像加载
	if (image.empty())
	{
		cout << "Couldn't read image filename" << filename << endl;
		return 1;
	}
	if (!image.data)
		return 0;

	
	GammaCorrection(image, 0.7);
	//Morphology(image);


	Mat Dst;
	Mat Dst_th;												//用于存储单通道图像
	pyrMeanShiftFiltering(image, Dst, msfsp, msfsr);		//进行meanshift滤波
	//Dst = image.clone();
	cvtColor(Dst, Dst_th, CV_BGR2GRAY);						//将原彩色图像转化为灰度图像
	//float th;
	//th = thratio*mean(Dst_th).val[0];						//根据平均灰度值乘自定义比例，获得阈值提取分解点
	equalizeHist(Dst_th, Dst_th);
	ThreIteration(Dst_th);
	namedWindow("dst_th", WINDOW_NORMAL);
	imshow("dst_th", Dst_th);

	//vector<Vec3f> circles;									//用于记录霍夫圆变换的结果，进行对比
	//HoughCircles(Dst_th, circles, CV_HOUGH_GRADIENT, 1, Dst.rows/8, 100, 20, 0, 0);
	//for (size_t i = 0; i < circles.size(); i++)
	//{
	//	Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	//	int radius = cvRound(circles[i][2]);
	//	// circle center
	//	circle(image, center, 3, Scalar(0, 255, 0), -1, 8, 0);
	//	// circle outline
	//	circle(image, center, radius, Scalar(0, 0, 255), 3, 8, 0);
	//}

	vector<vector<Point>> vecContour;
	vector<Vec4i> Hierachy;
	findContours(Dst_th, vecContour, Hierachy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	if (vecContour.size() <= 0)
	{
		cout << "failed to search the workpiece" << endl;
		return 0;
	}

	sort(vecContour.begin(), vecContour.end(), compareArea);
	vector<Point> Contour;
	Contour = *(vecContour.begin() + 1);		//选择其中面积第二大的轮廓作为工件轮廓

	Moments mu;									//计算轮廓矩	
	mu = moments(Contour, false);
	Point2f mc;									//计算轮廓质心
	mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);

	vector<vector<Point>> vectemp;				//用于轮廓描绘
	vectemp.clear();
	vectemp.push_back(Contour);
	drawContours(image, vectemp, -1, RED);
	circle(image, mc, 3, BLUE, -1);
	cout << "mc = " <<  mc << endl;

	//float theta;												//计算轮廓长轴与水平x轴之间的夹角
	//theta = 0.5*atan(2 * mu.nu11 / (mu.nu20 - mu.nu02));
	//line(image, Point(mc.x + image.cols, mc.y - image.cols * tan(theta)),
	//	Point(mc.x - image.cols, mc.y + image.cols * tan(theta)), GREEN, 2);

	namedWindow("Contours", CV_WINDOW_NORMAL);
	imshow("Contours", image);
	waitKey(0);
	
}

