#include "stdafx.h"
#include "ProImage.h"
ProImage::ProImage()
{
	path = "";
}

ProImage::~ProImage()
{

}

void ProImage::preproImage()
{
	medianBlur(srcimage, srcimage, 1);

	int morph_elem = 0;		//Element:\n 0: Rect - 1: Cross - 2: Ellipse
	int morph_size = 5;
	Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
	morphologyEx(srcimage, srcimage, MORPH_OPEN, element);
	imshow("Proimage", srcimage);
}

void ProImage::getContour()
{
	Mat conimage;
	cvtColor(srcimage, conimage, CV_BGR2GRAY);
	findContours(conimage, veccon, hierachy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<vector<Point>> polycon(veccon.size());
	for (int i = 0; i < polycon.size(); i++)
		approxPolyDP(veccon[i], polycon[i], 20, true);
	for(int i = 0; i < polycon.size(); i++)
		drawContours(conimage, polycon, i, Scalar(255, 0, 0), 1, 8, hierachy);
	imshow("contour", conimage);
}
