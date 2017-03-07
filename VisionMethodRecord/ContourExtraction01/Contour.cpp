#include "stdafx.h"
#include "Contour.h"
void Contour::Initialize(vector<Point> cont)
{
	this->contour = cont;
	this->area = contourArea(cont, false);
	//this->Box = boundingRect(cont);
	if (cont.size() >= 5)
	{
		this->minBox = fitEllipse(cont);
		radian_angle = this->minBox.angle / 180 * PI;
	}
	//this->center = Point2f(Box.x + Box.width / 2, Box.y + Box.height / 2);
}
void Contour::FetchPointPos(Point fetchpoint)
{
	//由2到1的变换矩阵：
	//12R = [cos(theta) -sin(theta)]
	//	  [sin(theta) cos(theta)]
	//由1到2的变换矩阵是上述矩阵的逆
	kpoint_org.x = fetchpoint.x - this->minBox.center.x;
	kpoint_org.y = fetchpoint.y - this->minBox.center.y;
	kpoint_tran.x = cos(radian_angle)*kpoint_org.x + sin(radian_angle)*kpoint_org.y;
	kpoint_tran.y = -sin(radian_angle)*kpoint_org.x + cos(radian_angle)*kpoint_org.y;
}

void Contour::FetchPointPosMapping(Contour src)
{
	float rate_x, rate_y;		//前后轮廓的缩放比例, dst/src
	rate_x = minBox.size.width / src.minBox.size.width;
	rate_y = minBox.size.height / src.minBox.size.height;
	kpoint_tran.x = src.kpoint_tran.x*rate_x;
	kpoint_tran.y = src.kpoint_tran.y*rate_y;
	kpoint_org.x = kpoint_tran.x*cos(radian_angle) - kpoint_tran.y*sin(radian_angle);
	kpoint_org.y = kpoint_tran.x*sin(radian_angle) +
		kpoint_tran.y*cos(radian_angle);
}

bool AreaCompare(Contour first, Contour second)
{
	if (first.area > second.area)
		return true;
	else return false;
}


