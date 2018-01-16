#include "stdafx.h"
#include <iostream>  
#include <stdio.h>  
#include "opencv2/core.hpp"  
#include "opencv2/core/utility.hpp"  
#include "opencv2/core/ocl.hpp"  
#include "opencv2/imgcodecs.hpp"  
#include "opencv2/highgui.hpp"  
#include "opencv2/features2d.hpp"  
#include "opencv2/calib3d.hpp"  
#include "opencv2/imgproc.hpp"  
#include"opencv2/flann.hpp"  
#include"opencv2/xfeatures2d.hpp"  
#include"opencv2/ml.hpp"  

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;
using namespace cv::ml;

int main()
{
	Mat a = imread("Surf1.JPG", IMREAD_GRAYSCALE);    //��ȡ�Ҷ�ͼ��  
	Mat b = imread("Surf2.JPG", IMREAD_GRAYSCALE);

	Ptr<SURF> surf;      //������ʽ��2�еĲ�һ��  
	surf = SURF::create(800);

	BFMatcher matcher;
	Mat c, d;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches;

	surf->detectAndCompute(a, Mat(), key1, c);
	surf->detectAndCompute(b, Mat(), key2, d);

	matcher.match(c, d, matches);       //ƥ��  
	//imshow("c", c);
	//imshow("d", d);
	//waitKey(0);

	sort(matches.begin(), matches.end());  //ɸѡƥ���  
	float max_dist = matches.back().distance, min_dist = matches.front().distance;
	vector< DMatch > good_matches;

	//int ptsPairs = std::min(50, (int)(matches.size() * 0.15));
	//cout << ptsPairs << endl;
	//for (int i = 0; i < ptsPairs; i++)
	//{
	//	good_matches.push_back(matches[i]);
	//}

	for (int i = 0; i < matches.size(); i++)
	{
		//if (matches[i].distance < 0.3*max_dist)
		if(matches[i].distance < 0.6*max_dist)
			good_matches.push_back(matches[i]);
		//if (good_matches.size() >= 200)
		//	break;
	}
	Mat outimg;
	drawMatches(a, key1, b, key2, good_matches, outimg, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //����ƥ���  

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (size_t i = 0; i < good_matches.size(); i++)
	{
		obj.push_back(key1[good_matches[i].queryIdx].pt);
		scene.push_back(key2[good_matches[i].trainIdx].pt);
	}

	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point(0, 0);
	obj_corners[1] = Point(a.cols, 0);
	obj_corners[2] = Point(a.cols, a.rows);
	obj_corners[3] = Point(0, a.rows);
	std::vector<Point2f> scene_corners(4);

	Mat H = findHomography(obj, scene, RANSAC);      //Ѱ��ƥ���ͼ��  
	//perspectiveTransform(obj_corners, scene_corners, H);

	//line(outimg, scene_corners[0] + Point2f((float)a.cols, 0), scene_corners[1] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);       //����  
	//line(outimg, scene_corners[1] + Point2f((float)a.cols, 0), scene_corners[2] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	//line(outimg, scene_corners[2] + Point2f((float)a.cols, 0), scene_corners[3] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	//line(outimg, scene_corners[3] + Point2f((float)a.cols, 0), scene_corners[0] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	namedWindow("result1", CV_WINDOW_NORMAL);
	imshow("result1", outimg);
	cvWaitKey(10);

	//RANSACƥ�����
	vector<DMatch> m_Matches = good_matches;
	int ptCount = (int)m_Matches.size();		//����ռ�
	if (ptCount < 100)
	{
		cout << "û���㹻��ƥ���" << endl;
		waitKey(0);
		return 0;
	}
	//��keypointת��ΪMat
	Point2f pt;
	Mat p1(ptCount, 2, CV_32F);
	Mat p2(ptCount, 2, CV_32F);
	for (int i = 0; i < ptCount; i++)
	{
		pt = key1[m_Matches[i].queryIdx].pt;
		p1.at<float>(i, 0) = pt.x;
		p1.at<float>(i, 1) = pt.y;

		pt = key2[m_Matches[i].trainIdx].pt;
		p2.at<float>(i, 0) = pt.x;
		p2.at<float>(i, 1) = pt.y;
	}
	//��RANSAC���������������F
	Mat m_Fundamental;
	vector<uchar> m_RANSACStatus;
	findFundamentalMat(p1, p2, m_RANSACStatus, FM_RANSAC);

	//����Ұ�����
	int OutLinerCount = 0;
	for (int i = 0; i < ptCount; i++)
	{
		if (m_RANSACStatus[i] == 0)		//��ʾҰ��
		{
			OutLinerCount++;
		}
	}
	int InlinerCount = ptCount - OutLinerCount;
	cout << "�ڵ���Ϊ��" << InlinerCount << endl;

	// �������������ڱ����ڵ��ƥ���ϵ   
	vector<Point2f> m_LeftInlier;
	vector<Point2f> m_RightInlier;
	vector<DMatch> m_InlierMatches;
	//������ƥ��Ϊ�ڵ�������С
	m_InlierMatches.resize(InlinerCount);
	m_LeftInlier.resize(InlinerCount);
	m_RightInlier.resize(InlinerCount);
	InlinerCount = 0;
	float inlier_minRx = a.cols;        //���ڴ洢�ڵ�����ͼ��С�����꣬�Ա�����ں�  

	for (int i = 0; i < ptCount; i++)
	{
		if (m_RANSACStatus[i] != 0)
		{
			m_LeftInlier[InlinerCount].x = p1.at<float>(i, 0);
			m_LeftInlier[InlinerCount].y = p1.at<float>(i, 1);
			m_RightInlier[InlinerCount].x = p2.at<float>(i, 0);
			m_RightInlier[InlinerCount].y = p2.at<float>(i, 1);
			m_InlierMatches[InlinerCount].queryIdx = InlinerCount;
			m_InlierMatches[InlinerCount].trainIdx = InlinerCount;
			if (m_RightInlier[InlinerCount].x<inlier_minRx) 
				inlier_minRx = m_RightInlier[InlinerCount].x;   //�洢�ڵ�����ͼ��С������   
			InlinerCount++;
		}
	}
	
	// ���ڵ�ת��ΪdrawMatches����ʹ�õĸ�ʽ   
	vector<KeyPoint> key1_RANSAC(InlinerCount);
	vector<KeyPoint> key2_RANSAC(InlinerCount);
	KeyPoint::convert(m_LeftInlier, key1_RANSAC);
	KeyPoint::convert(m_RightInlier, key2_RANSAC);

	// ��ʾ����F������ڵ�ƥ��   
	Mat OutImage;
	drawMatches(a, key1_RANSAC, b, key2_RANSAC, m_InlierMatches, OutImage);
	namedWindow("RANSAC result2", CV_WINDOW_NORMAL);
	imshow("RANSAC result2", OutImage);
	waitKey(0);
}