#ifndef _CALCLOC_H_
#define _CALCLOC_H_
#include "ProImage.h"
#include <fstream>
#include <time.h>
class CalcLoc : public ProImage
{
public:
	CalcLoc();
	~CalcLoc();
	void getHUMoment();
	void getCenter();
	static void printResult(CalcLoc &dst);
	static void printResult(time_t nowtime);
private:
	vector<Point2f> m_Pcenter;		//多边形中心
	vector<Point2f> m_Ecenter;		//椭圆中心
	Point2f m_center;
	vector<Moments> vecmu;
};
#endif

