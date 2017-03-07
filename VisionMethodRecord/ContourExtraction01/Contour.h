#pragma once
#include <iostream>  
#include <vector>  
#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include "algorithm"
using namespace std;
using namespace cv;

#define PI 3.141592654;

//该类用于存储相应轮廓的点序列、面积、中心等
class Contour
{
public:
	Contour() {}
	void Initialize(vector<Point> cont);	
	void FetchPointPos(Point fetchpoint);						//计算fetchpoint与椭圆拟合矩形在拟合坐标系中的相对位置
	void FetchPointPosMapping(Contour src);		//计算将src中的fetchpoint的相对位置映射到dst中，并进一步反映到原始坐标系中的相对位置

	vector<Point> contour;	//轮廓
	double area;			//轮廓面积
	Vec4i hierachy;			//轮廓等级

	Rect Box;				//轮廓包围矩形
	RotatedRect minBox;		//轮廓最小包围矩形
	//Point2f vertices[4];	//包围矩形的顶点
	//Point2f center;			//包围矩形的中心
	float radian_angle;		//轮廓拟合椭圆旋转的角度（弧度）
	//int fetchpoint_dx;		//抓取点fetchpoint与轮廓椭圆拟合中心在椭圆拟合坐标系x方向上的差
	//int fetchpoint_dy;		//抓取点fetchpoint与轮廓椭圆拟合中心在椭圆拟合坐标系y方向上的差
	//int fetchpoint_dx_o;		//抓取点fetchpoint与轮廓椭圆拟合中心在原图像坐标系x方向上的差
	//int fetchpoint_dy_o;		//抓取点fetchpoint与轮廓椭圆拟合中心在团图像坐标系y方向上的差
	Point2f kpoint_tran;		//fetchpoint transformed在椭圆拟合坐标系中的相对椭圆圆心位置
	Point2f kpoint_org;		//fetchpoint origin在原图像坐标系中的相对椭圆圆心位置
};

bool AreaCompare(Contour first, Contour second); //用于比较前后轮廓面积的大小