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
	Mat a = imread("Surf1.JPG", IMREAD_GRAYSCALE);    //读取灰度图像  
	Mat b = imread("Surf2.JPG", IMREAD_GRAYSCALE);

	Ptr<SURF> surf;      //创建方式和2中的不一样  
	surf = SURF::create(800);

	BFMatcher matcher;
	Mat c, d;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches;

	surf->detectAndCompute(a, Mat(), key1, c);
	surf->detectAndCompute(b, Mat(), key2, d);

	matcher.match(c, d, matches);       //匹配  

	sort(matches.begin(), matches.end());  //筛选匹配点  
	float max_dist = matches.back().distance, min_dist = matches.front().distance;
	vector< DMatch > good_matches;

	//完成surf匹配，使用ratio test首次去除误匹配点

	for (int i = 0; i < matches.size(); i++)
	{
		//if (matches[i].distance < 0.3*max_dist)
		if (matches[i].distance < 0.6*max_dist)
			good_matches.push_back(matches[i]);
		//if (good_matches.size() >= 200)
		//	break;
	}
	//Mat outimg;
	//drawMatches(a, key1, b, key2, good_matches, outimg, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点  

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (size_t i = 0; i < good_matches.size(); i++)
	{
		obj.push_back(key1[good_matches[i].queryIdx].pt);
		scene.push_back(key2[good_matches[i].trainIdx].pt);
	}
	//相机内参数矩阵的定义
	double cameraMatrix[3][3] = { { 1120.93705, 0, 497.89122 },{ 0, 1120.43729, 374.34666 },{ 0, 0, 1 } };
	Mat m_cameraMatrix = Mat(3, 3, CV_64F, cameraMatrix);

	//根据内参矩阵获取相机的焦距和光心坐标（主点坐标）
	double focal_length = 0.5*(m_cameraMatrix.at<double>(0) + m_cameraMatrix.at<double>(4));
	Point2d principle_point(m_cameraMatrix.at<double>(2), m_cameraMatrix.at<double>(5));

	//vector<uchar> m_RANSACStatus;
	Mat m_RANSACStatus = Mat(obj.size(), 1, CV_8UC1);
	Mat E = findEssentialMat(obj, scene, focal_length, principle_point, RANSAC, 0.999, 1.0, m_RANSACStatus);
	if (E.empty()) 
		return 0;

	double feasible_count = countNonZero(m_RANSACStatus);
	cout << (int)feasible_count << " -in- " << obj.size() << endl;
	//对于RANSAC而言，outlier数量大于50%时，结果是不可靠的
	//if (feasible_count <= 15 || (feasible_count / obj.size()) < 0.6)
	//	return false;
	
	//分解本征矩阵，获取相对变换
	Mat R = Mat::eye(3, 3, CV_32FC1);
	Mat T = Mat(3, 1, CV_32FC1);		//相机的旋转平移矩阵
	int pass_count = recoverPose(E, obj, scene, R, T, focal_length, principle_point, m_RANSACStatus);

	//同时位于两个相机前方的点的数量要足够大
	if (((double)pass_count) / feasible_count < 0.7)
		return false;

	return true;

	//std::vector<Point2f> obj_corners(4);
	//obj_corners[0] = Point(0, 0);
	//obj_corners[1] = Point(a.cols, 0);
	//obj_corners[2] = Point(a.cols, a.rows);
	//obj_corners[3] = Point(0, a.rows);
	//std::vector<Point2f> scene_corners(4);

	//Mat H = findHomography(obj, scene, RANSAC);      //寻找匹配的图像  
	//perspectiveTransform(obj_corners, scene_corners, H);

	//line(outimg, scene_corners[0] + Point2f((float)a.cols, 0), scene_corners[1] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);       //绘制  
	//line(outimg, scene_corners[1] + Point2f((float)a.cols, 0), scene_corners[2] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	//line(outimg, scene_corners[2] + Point2f((float)a.cols, 0), scene_corners[3] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	//line(outimg, scene_corners[3] + Point2f((float)a.cols, 0), scene_corners[0] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	//namedWindow("result1", CV_WINDOW_NORMAL);
	//imshow("result1", outimg);
	//cvWaitKey(10);

	//RANSAC匹配过程
	//vector<DMatch> m_Matches = good_matches;
	//int ptCount = (int)m_Matches.size();		//分配空间
	//if (ptCount < 100)
	//{
	//	cout << "没有足够的匹配点" << endl;
	//	waitKey(0);
	//	return 0;
	//}
	////将keypoint转换为Mat
	//Point2f pt;
	//Mat p1(ptCount, 2, CV_32F);
	//Mat p2(ptCount, 2, CV_32F);
	//for (int i = 0; i < ptCount; i++)
	//{
	//	/*pt = key1[m_Matches[i].queryIdx].pt;
	//	p1.at<float>(i, 0) = pt.x;
	//	p1.at<float>(i, 1) = pt.y;*/
	//	p1.at<float>(i, 0) = obj[i].x;
	//	p1.at<float>(i, 1) = obj[i].y;

	//	/*pt = key2[m_Matches[i].trainIdx].pt;
	//	p2.at<float>(i, 0) = pt.x;
	//	p2.at<float>(i, 1) = pt.y;*/
	//	p2.at<float>(i, 0) = scene[i].x;
	//	p2.at<float>(i, 1) = scene[i].y;
	//}
	////用RANSAC方法计算本征矩阵
	//Mat m_Fundamental;
	//vector<uchar> m_RANSACStatus;
	//findFundamentalMat(obj, scene, m_RANSACStatus, FM_RANSAC);

	////计算野点个数
	//int OutLinerCount = 0;
	//for (int i = 0; i < ptCount; i++)
	//{
	//	if (m_RANSACStatus[i] == 0)		//表示野点
	//	{
	//		OutLinerCount++;
	//	}
	//}
	//int InlinerCount = ptCount - OutLinerCount;
	//cout << "内点数为：" << InlinerCount << endl;

	//// 这三个变量用于保存内点和匹配关系   
	//vector<Point2f> m_LeftInlier;
	//vector<Point2f> m_RightInlier;
	//vector<DMatch> m_InlierMatches;
	////将变量匹配为内点数量大小
	//m_InlierMatches.resize(InlinerCount);
	//m_LeftInlier.resize(InlinerCount);
	//m_RightInlier.resize(InlinerCount);
	//InlinerCount = 0;
	//float inlier_minRx = a.cols;        //用于存储内点中右图最小横坐标，以便后续融合  

	//for (int i = 0; i < ptCount; i++)
	//{
	//	if (m_RANSACStatus[i] != 0)
	//	{
	//		m_LeftInlier[InlinerCount].x = p1.at<float>(i, 0);
	//		m_LeftInlier[InlinerCount].y = p1.at<float>(i, 1);
	//		m_RightInlier[InlinerCount].x = p2.at<float>(i, 0);
	//		m_RightInlier[InlinerCount].y = p2.at<float>(i, 1);
	//		m_InlierMatches[InlinerCount].queryIdx = InlinerCount;
	//		m_InlierMatches[InlinerCount].trainIdx = InlinerCount;
	//		if (m_RightInlier[InlinerCount].x<inlier_minRx)
	//			inlier_minRx = m_RightInlier[InlinerCount].x;   //存储内点中右图最小横坐标   
	//		InlinerCount++;
	//	}
	//}

	//// 把内点转换为drawMatches可以使用的格式   
	//vector<KeyPoint> key1_RANSAC(InlinerCount);
	//vector<KeyPoint> key2_RANSAC(InlinerCount);
	//KeyPoint::convert(m_LeftInlier, key1_RANSAC);
	//KeyPoint::convert(m_RightInlier, key2_RANSAC);

	//// 显示计算F过后的内点匹配   
	//Mat OutImage;
	//drawMatches(a, key1_RANSAC, b, key2_RANSAC, m_InlierMatches, OutImage);
	//namedWindow("RANSAC result2", CV_WINDOW_NORMAL);
	//imshow("RANSAC result2", OutImage);
	//waitKey(0);
}