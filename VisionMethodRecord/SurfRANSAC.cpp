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
#include "opencv2/nofree.hpp"

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;
using namespace cv::ml;

int main(int argc, char** argv)
{
	initModule_nonfree();//��ʼ��ģ�飬ʹ��SIFT��SURFʱ�õ�    
	Ptr<FeatureDetector> detector = FeatureDetector::create("SURF");//����SIFT������������ɸĳ�SURF/ORB   
	Ptr<DescriptorExtractor> descriptor_extractor = DescriptorExtractor::create("SURF");//���������������������ɸĳ�SURF/ORB   
	Ptr<DescriptorMatcher> descriptor_matcher = DescriptorMatcher::create("BruteForce");//��������ƥ����     
	if (detector.empty() || descriptor_extractor.empty())
		cout << "fail to create detector!";

	//����ͼ��     
	Mat img1 = imread("E:/Book/3.jpg");
	Mat img2 = imread("E:/Book/4.jpg");

	Size imgSize(320, 240);
	resize(img1, img1, imgSize);
	resize(img2, img2, imgSize);

	//��������     
	double t = getTickCount();//��ǰ�δ���     
	vector<KeyPoint> m_LeftKey, m_RightKey;
	detector->detect(img1, m_LeftKey);//���img1�е�SIFT�����㣬�洢��m_LeftKey��     
	detector->detect(img2, m_RightKey);
	cout << "ͼ��1���������:" << m_LeftKey.size() << endl;
	cout << "ͼ��2���������:" << m_RightKey.size() << endl;

	//����������������������Ӿ��󣬼�������������     
	Mat descriptors1, descriptors2;
	descriptor_extractor->compute(img1, m_LeftKey, descriptors1);
	descriptor_extractor->compute(img2, m_RightKey, descriptors2);
	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "SIFT�㷨��ʱ��" << t << "��" << endl;

	cout << "ͼ��1�������������С��" << descriptors1.size()
		<< "����������������" << descriptors1.rows << "��ά����" << descriptors1.cols << endl;
	cout << "ͼ��2�������������С��" << descriptors2.size()
		<< "����������������" << descriptors2.rows << "��ά����" << descriptors2.cols << endl;

	//����������     
	Mat img_m_LeftKey, img_m_RightKey;
	drawKeypoints(img1, m_LeftKey, img_m_LeftKey, Scalar::all(-1), 0);
	drawKeypoints(img2, m_RightKey, img_m_RightKey, Scalar::all(-1), 0);
	//imshow("Src1",img_m_LeftKey);     
	//imshow("Src2",img_m_RightKey);     

	//����ƥ��     
	vector<DMatch> matches;//ƥ����     
	descriptor_matcher->match(descriptors1, descriptors2, matches);//ƥ������ͼ�����������     
	cout << "Match������" << matches.size() << endl;

	//����ƥ�����о����������Сֵ     
	//������ָ���������������ŷʽ���룬�������������Ĳ��죬ֵԽС��������������Խ�ӽ�     
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i<matches.size(); i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	cout << "�����룺" << max_dist << endl;
	cout << "��С���룺" << min_dist << endl;

	//ɸѡ���Ϻõ�ƥ���     
	vector<DMatch> goodMatches;
	for (int i = 0; i<matches.size(); i++)
	{
		if (matches[i].distance < 0.6 * max_dist)
		{
			goodMatches.push_back(matches[i]);
		}
	}
	cout << "goodMatch������" << goodMatches.size() << endl;

	//����ƥ����     
	Mat img_matches;
	//��ɫ���ӵ���ƥ���������ԣ���ɫ��δƥ���������     
	drawMatches(img1, m_LeftKey, img2, m_RightKey, goodMatches, img_matches,
		Scalar::all(-1)/*CV_RGB(255,0,0)*/, CV_RGB(0, 255, 0), Mat(), 2);

	imshow("MatchSIFT", img_matches);
	IplImage result = img_matches;

	waitKey(10);


	//RANSACƥ�����   
	vector<DMatch> m_Matches = goodMatches;
	// ����ռ�   
	int ptCount = (int)m_Matches.size();

	if (ptCount<100)
	{
		cout << "û���ҵ��㹻��ƥ���" << endl;
		waitKey(0);
		return 0;
	}
	Mat p1(ptCount, 2, CV_32F);
	Mat p2(ptCount, 2, CV_32F);

	// ��Keypointת��ΪMat   
	Point2f pt;
	for (int i = 0; i<ptCount; i++)
	{
		pt = m_LeftKey[m_Matches[i].queryIdx].pt;
		p1.at<float>(i, 0) = pt.x;
		p1.at<float>(i, 1) = pt.y;

		pt = m_RightKey[m_Matches[i].trainIdx].pt;
		p2.at<float>(i, 0) = pt.x;
		p2.at<float>(i, 1) = pt.y;
	}

	// ��RANSAC��������F   
	Mat m_Fundamental;
	vector<uchar> m_RANSACStatus;       // ����������ڴ洢RANSAC��ÿ�����״̬   
	findFundamentalMat(p1, p2, m_RANSACStatus, FM_RANSAC);

	// ����Ұ�����   

	int OutlinerCount = 0;
	for (int i = 0; i<ptCount; i++)
	{
		if (m_RANSACStatus[i] == 0)    // ״̬Ϊ0��ʾҰ��   
		{
			OutlinerCount++;
		}
	}
	int InlinerCount = ptCount - OutlinerCount;   // �����ڵ�   
	cout << "�ڵ���Ϊ��" << InlinerCount << endl;


	// �������������ڱ����ڵ��ƥ���ϵ   
	vector<Point2f> m_LeftInlier;
	vector<Point2f> m_RightInlier;
	vector<DMatch> m_InlierMatches;

	m_InlierMatches.resize(InlinerCount);
	m_LeftInlier.resize(InlinerCount);
	m_RightInlier.resize(InlinerCount);
	InlinerCount = 0;
	float inlier_minRx = img1.cols;        //���ڴ洢�ڵ�����ͼ��С�����꣬�Ա�����ں�   

	for (int i = 0; i<ptCount; i++)
	{
		if (m_RANSACStatus[i] != 0)
		{
			m_LeftInlier[InlinerCount].x = p1.at<float>(i, 0);
			m_LeftInlier[InlinerCount].y = p1.at<float>(i, 1);
			m_RightInlier[InlinerCount].x = p2.at<float>(i, 0);
			m_RightInlier[InlinerCount].y = p2.at<float>(i, 1);
			m_InlierMatches[InlinerCount].queryIdx = InlinerCount;
			m_InlierMatches[InlinerCount].trainIdx = InlinerCount;

			if (m_RightInlier[InlinerCount].x<inlier_minRx) inlier_minRx = m_RightInlier[InlinerCount].x;   //�洢�ڵ�����ͼ��С������   

			InlinerCount++;
		}
	}

	// ���ڵ�ת��ΪdrawMatches����ʹ�õĸ�ʽ   
	vector<KeyPoint> key1(InlinerCount);
	vector<KeyPoint> key2(InlinerCount);
	KeyPoint::convert(m_LeftInlier, key1);
	KeyPoint::convert(m_RightInlier, key2);

	// ��ʾ����F������ڵ�ƥ��   
	Mat OutImage;
	drawMatches(img1, key1, img2, key2, m_InlierMatches, OutImage);
	cvNamedWindow("Match features", 1);
	cvShowImage("Match features", &IplImage(OutImage));
	waitKey(10);

	cvDestroyAllWindows();

	//����H���Դ洢RANSAC�õ��ĵ�Ӧ����   
	Mat H = findHomography(m_LeftInlier, m_RightInlier, RANSAC);

	//�洢��ͼ�Ľǣ�����任����ͼλ��   
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point(0, 0); obj_corners[1] = Point(img1.cols, 0);
	obj_corners[2] = Point(img1.cols, img1.rows); obj_corners[3] = Point(0, img1.rows);
	std::vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);

	//�����任��ͼ��λ��   
	Point2f offset((float)img1.cols, 0);
	line(OutImage, scene_corners[0] + offset, scene_corners[1] + offset, Scalar(0, 255, 0), 4);
	line(OutImage, scene_corners[1] + offset, scene_corners[2] + offset, Scalar(0, 255, 0), 4);
	line(OutImage, scene_corners[2] + offset, scene_corners[3] + offset, Scalar(0, 255, 0), 4);
	line(OutImage, scene_corners[3] + offset, scene_corners[0] + offset, Scalar(0, 255, 0), 4);
	imshow("Good Matches & Object detection", OutImage);

	waitKey(10);
	imwrite("warp_position.jpg", OutImage);


	int drift = scene_corners[1].x;                                                        //����ƫ����   

																						   //�½�һ������洢��׼���Ľǵ�λ��   
	int width = int(max(abs(scene_corners[1].x), abs(scene_corners[2].x)));
	int height = img1.rows;                                                                  //���ߣ�int height = int(max(abs(scene_corners[2].y), abs(scene_corners[3].y)));   
	float origin_x = 0, origin_y = 0;
	if (scene_corners[0].x<0) {
		if (scene_corners[3].x<0) origin_x += min(scene_corners[0].x, scene_corners[3].x);
		else origin_x += scene_corners[0].x;
	}
	width -= int(origin_x);
	if (scene_corners[0].y<0) {
		if (scene_corners[1].y) origin_y += min(scene_corners[0].y, scene_corners[1].y);
		else origin_y += scene_corners[0].y;
	}
	//��ѡ��height-=int(origin_y);   
	Mat imageturn = Mat::zeros(width, height, img1.type());

	//��ȡ�µı任����ʹͼ��������ʾ   
	for (int i = 0;i<4;i++) { scene_corners[i].x -= origin_x; }     //��ѡ��scene_corners[i].y -= (float)origin_y; }   
	Mat H1 = getPerspectiveTransform(obj_corners, scene_corners);

	//����ͼ��任����ʾЧ��   
	warpPerspective(img1, imageturn, H1, Size(width, height));
	imshow("image_Perspective", imageturn);
	waitKey(10);


	//ͼ���ں�   
	int width_ol = width - int(inlier_minRx - origin_x);
	int start_x = int(inlier_minRx - origin_x);
	cout << "width: " << width << endl;
	cout << "img1.width: " << img1.cols << endl;
	cout << "start_x: " << start_x << endl;
	cout << "width_ol: " << width_ol << endl;

	uchar* ptr = imageturn.data;
	double alpha = 0, beta = 1;
	for (int row = 0;row<height;row++)
	{
		ptr = imageturn.data + row*imageturn.step + (start_x)*imageturn.elemSize();
		for (int col = 0;col<width_ol;col++)
		{
			uchar* ptr_c1 = ptr + imageturn.elemSize1();
			uchar*  ptr_c2 = ptr_c1 + imageturn.elemSize1();
			uchar* ptr2 = img2.data + row*img2.step + (col + int(inlier_minRx))*img2.elemSize();
			uchar* ptr2_c1 = ptr2 + img2.elemSize1();
			uchar* ptr2_c2 = ptr2_c1 + img2.elemSize1();

			alpha = double(col) / double(width_ol); beta = 1 - alpha;

			if (*ptr == 0 && *ptr_c1 == 0 && *ptr_c2 == 0)
			{
				*ptr = (*ptr2);
				*ptr_c1 = (*ptr2_c1);
				*ptr_c2 = (*ptr2_c2);
			}

			*ptr = (*ptr)*beta + (*ptr2)*alpha;
			*ptr_c1 = (*ptr_c1)*beta + (*ptr2_c1)*alpha;
			*ptr_c2 = (*ptr_c2)*beta + (*ptr2_c2)*alpha;

			ptr += imageturn.elemSize();
		}
	}

	//imshow("image_overlap", imageturn);   
	//waitKey(0);   

	Mat img_result = Mat::zeros(height, width + img2.cols - drift, img1.type());
	uchar* ptr_r = imageturn.data;

	for (int row = 0;row<height;row++)
	{
		ptr_r = img_result.data + row*img_result.step;

		for (int col = 0;col<img_result.cols;col++)
		{
			uchar* ptr_rc1 = ptr_r + imageturn.elemSize1();
			uchar* ptr_rc2 = ptr_rc1 + imageturn.elemSize1();

			uchar* ptr = imageturn.data + row*imageturn.step + col*imageturn.elemSize();
			uchar* ptr_c1 = ptr + imageturn.elemSize1();
			uchar* ptr_c2 = ptr_c1 + imageturn.elemSize1();

			*ptr_r = *ptr;
			*ptr_rc1 = *ptr_c1;
			*ptr_rc2 = *ptr_c2;

			ptr_r += img_result.elemSize();
		}

		ptr_r = img_result.data + row*img_result.step + imageturn.cols*img_result.elemSize();
		for (int col = imageturn.cols;col<img_result.cols;col++)
		{
			uchar* ptr_rc1 = ptr_r + imageturn.elemSize1();
			uchar*  ptr_rc2 = ptr_rc1 + imageturn.elemSize1();

			uchar* ptr2 = img2.data + row*img2.step + (col - imageturn.cols + drift)*img2.elemSize();
			uchar* ptr2_c1 = ptr2 + img2.elemSize1();
			uchar* ptr2_c2 = ptr2_c1 + img2.elemSize1();

			*ptr_r = *ptr2;
			*ptr_rc1 = *ptr2_c1;
			*ptr_rc2 = *ptr2_c2;

			ptr_r += img_result.elemSize();
		}
	}

	//imshow("image_result", img_result);  
	//imwrite("final_result.jpg",img_result);  
	waitKey(0);

	return 0;
}