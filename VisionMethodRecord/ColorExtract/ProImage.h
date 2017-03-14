#ifndef _PROIMAGE_H_
#define _PROIMAGE_H_
#include "HSVHist.h"
using namespace std;
using namespace cv;
class ProImage:public HSVHist
{
public:
	ProImage();
	~ProImage();
	void preproImage();
	void getContour();
	void calcLoc();
private:
	Point center;
	vector<vector<Point>> veccon;
	vector<Vec4i> hierachy;
};
#endif