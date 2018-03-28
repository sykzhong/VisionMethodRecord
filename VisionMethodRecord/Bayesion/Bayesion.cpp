#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/core/core.hpp"
#include <time.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "easylogging++.h"
#define TEST1
INITIALIZE_EASYLOGGINGPP
using namespace cv;
using namespace std;
using namespace cv::ml;
using namespace el;

static const Scalar RED = Scalar(0, 0, 255);
static const Scalar PINK = Scalar(230, 130, 255);
static const Scalar BLUE = Scalar(255, 0, 0);
static const Scalar LIGHTBLUE = Scalar(255, 255, 160);
static const Scalar GREEN = Scalar(0, 255, 0);
static const Scalar BLACK = Scalar(0, 0, 0);
static const Scalar WHITE = Scalar(255, 255, 255);

int Hbin = 1;
int Sbin = 1;
int Vbin = 1;
int Ibin = 1;
vector<int> Hclass;
vector<int> Sclass;
vector<int> Vclass;
vector<int> Iclass;

void calcHSVComponent(Mat &image, Mat &mask = Mat())
{
	if (!mask.data || mask.size() != image.size())
		mask = Mat(image.size(), CV_8UC1, Scalar::all(255));

	Hclass.clear();
	Hclass.resize(180 / Hbin);
	fill(Hclass.begin(), Hclass.end(), 0);

	Sclass.clear();
	Sclass.resize(256 / Sbin);
	fill(Sclass.begin(), Sclass.end(), 0);

	Vclass.clear();
	Vclass.resize(256 / Vbin);
	fill(Vclass.begin(), Vclass.end(), 0);

	int nChannels = image.channels();
	int nRows = image.rows;
	int nCols = image.cols;
	if (image.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *p;		//image的行指针
	uchar *q;		//mask的行指针
	int Hindex = 0;
	int Sindex = 0;
	int Vindex = 0;
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			q = mask.ptr<uchar>(i);
			if (q[j] == 0)		//根据掩码进行像素处理选择
				continue;
			p = image.ptr<uchar>(i);
			Hindex = p[j*nChannels] / Hbin;
			Sindex = p[j*nChannels + 1] / Sbin;
			Vindex = p[j*nChannels + 2] / Vbin;
			Hclass[Hindex] += 1;
			Sclass[Sindex] += 1;
			Vclass[Vindex] += 1;
		}
}

void calcIComponent(Mat &src, Mat& mask = Mat())
{
	if (!mask.data || mask.size() != src.size())
		mask = Mat(src.size(), CV_8UC1, Scalar::all(255));

	Mat image = src.clone();
	cvtColor(image, image, CV_HSV2BGR);

	//I取值范围（-1.192, 1.192）*256，记为306
	Iclass.clear();
	Iclass.resize(306 / Ibin);
	fill(Iclass.begin(), Iclass.end(), 0);

	int nChannels = image.channels();
	int nRows = image.rows;
	int nCols = image.cols;
	if (image.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *p;
	uchar *q;
	uchar r, g, b;
	double Y, I, Q;
	int Iindex = 0;
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			q = mask.ptr<uchar>(i);
			if (q[j] == 0)		//根据掩码进行像素处理选择
				continue;
			p = image.ptr<uchar>(i);
			b = p[j*nChannels];
			g = p[j*nChannels + 1];
			r = p[j*nChannels + 2];
			Y = 0.299*r + 0.587*g + 0.114*b;
			I = 0.596*r - 0.275*g - 0.321*b + 153;		//I取值为-153~153，需将其转换为正数
			Q = 0.212*r - 0.523*g + 0.311*b;
			Iindex = I / Ibin;
			Iclass[Iindex] += 1;
		}
}

void saveData(int &_Hbin, vector<int>&_Hclass, const string &filename = "")
{
	ofstream result;
	string path = "";
	if (filename == "")
		path = "result.txt";
	else
		
		path = filename;
	result.open(path, ios::out | ios::ate);
	time_t nowtime;
	nowtime = time(NULL);
	for (int i = 0; i < _Hclass.size(); i++)
	{
		result << _Hbin * i << " " << _Hclass[i] << endl;
	}
}

void drawData(int &_Hbin, vector<int> &_Hclass, const string &filename = "")
{
	//绘制的图像高,宽,单格宽
	int height = 400;
	double hratio = 0.8;		//最高者只能占总高的比例
	//int width = 400;
	//int w_bin = round(width / _Hclass.size());
	int w_bin = 2;
	int width = w_bin*_Hclass.size();
	Mat image(height, width, CV_8UC3);
	
	int maxheight = *max_element(_Hclass.begin(), _Hclass.end());
	for (int i = 0; i < _Hclass.size(); i++)
	{
		int tmpheight = round(_Hclass[i] * (height*hratio) / maxheight);
		circle(image, Point(w_bin*(i + 1), height - tmpheight), 2, RED, -1);
		if (i != 0)
		{
			int pretmpheight = round(_Hclass[i - 1] * (height*hratio) / maxheight);
			line(image, Point(w_bin*(i + 1), height - tmpheight), Point(w_bin*i, height - pretmpheight), RED, 1);
		}
			
	}
	string path;
	if (filename == "")
		path = "result";
	else
		path = filename;
	imshow(path, image);
}

/**************************************************************************************************************/

/*将image转换为HI参数，并按(image.rows*image.cols, 2)的大小映射至trainingData中*/
void getTrainingData(Mat &image, Mat &trainingData)	
{
	//image为HSV图像
	int nRows = image.rows;
	int nCols = image.cols;
	trainingData = Mat(image.rows*image.cols, 4, CV_32FC1);
	int nChannels = image.channels();
	if (image.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	Mat bgrimage = Mat(image.size(), CV_8UC3);
	cvtColor(image, bgrimage, CV_HSV2BGR);
	uchar *pimage;			//原图像的行指针
	uchar *pbgrimage;		//转换为BGR图像的行指针
	uchar *ptrain;			//
	int b, g, r;
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			pimage = image.ptr<uchar>(i);
			pbgrimage = bgrimage.ptr<uchar>(i);
			b = pbgrimage[j*nChannels];
			g = pbgrimage[j*nChannels + 1];
			r = pbgrimage[j*nChannels + 2];

			trainingData.at<float>(i*nCols + j, 0) = (float)pimage[j*nChannels] / Hbin;
			trainingData.at<float>(i*nCols + j, 1) = (float)pimage[j*nChannels + 1] / Sbin;
			trainingData.at<float>(i*nCols + j, 2) = (float)pimage[j*nChannels + 2] / Vbin;
			trainingData.at<float>(i*nCols + j, 3) = 0.596*r - 0.275*g - 0.321*b;

			//ptrain = trainingData.ptr<uchar>(i);
			//ptrain[j*trainChannels] = pimage[j*imageChannels] / Hbin;
			//ptrain[j*trainChannels + 1] = 0.596*r - 0.275*g - 0.321*b;
		}
}

void getResponseData(Mat &mask, Mat &responseData)
{
	int nRows = mask.rows;
	int nCols = mask.cols;
	responseData = Mat(mask.rows*mask.cols, 1, CV_32SC1);
	//responseData = Mat(1, mask.rows*mask.cols, CV_32FC1);
	int nChannels = mask.channels();
	if (mask.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *p;
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			p = mask.ptr<uchar>(i);
			if (p[j*nChannels] > 0)
				responseData.at<float>(i*nCols + j, 0) = 1;
				//responseData.at<float>(0, i*nCols + j) = 1;
			else
				responseData.at<float>(i*nCols + j, 0) = 0;
				//responseData.at<float>(0, i*nCols + j) = 0;
		}
}

void getTrainResult(NormalBayesClassifier *nbc, Mat &dstData, Mat &result)
{
	int nRows = result.rows;
	int nCols = result.cols;
	int nChannels = result.channels();
	if (result.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	Mat dataNode = Mat(1, 4, CV_32FC1);			//用于dstData(目标图像参数空间)的结果
	uchar *q;
	
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			q = result.ptr<uchar>(i);
			dataNode.at<float>(0, 0) = dstData.at<float>(i*nCols + j, 0);
			dataNode.at<float>(0, 1) = dstData.at<float>(i*nCols + j, 1);
			dataNode.at<float>(0, 2) = dstData.at<float>(i*nCols + j, 2);
			dataNode.at<float>(0, 3) = dstData.at<float>(i*nCols + j, 3);
			//LOG(TRACE) << dataNode;
			if (nbc->predict(dataNode) != 0)
				q[j*nChannels] = 255;
			else
				q[j*nChannels] = 0;
		}
}
#ifdef TEST1				//单张图的训练
int main()
{
	Configurations conf("my-conf.conf");
	Loggers::reconfigureLogger("default", conf);
	LOG(TRACE) << "Begin";

	Mat src = imread("2.bmp");
	Mat mask = imread("2syk.jpg", 0);		//src mask像素不为零的区域为工件区域
	//Mat src = imread("23.jpg");
	//Mat mask = imread("23syk.jpg", 0);		//src mask像素不为零的区域为工件区域
	cvtColor(src, src, CV_BGR2HSV);
	threshold(mask, mask, 254, 255, THRESH_BINARY_INV);	//Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted
	//imshow("mask", mask);

	/*获取训练样本*/
	Mat trainingData;			//训练样本
	Mat responseData;			//训练样本
	getTrainingData(src, trainingData);
	getResponseData(mask, responseData);
	//LOG(INFO) << responseData << endl;
	/**************/

	Ptr<NormalBayesClassifier> nbc = NormalBayesClassifier::create();
	Ptr<TrainData> tData = TrainData::create(trainingData, ROW_SAMPLE, responseData);
	nbc->train(tData);

	Mat dst = imread("4.bmp");
	cvtColor(dst, dst, CV_BGR2HSV);

	Mat dstData;
	getTrainingData(dst, dstData);

	Mat result = Mat(dst.size(), CV_8UC1);
	getTrainResult(nbc, dstData, result);
	imshow("result", result);
	//imwrite("Bayesion/23result.jpg", result);
	imwrite("Bayesion/2result.jpg", result);

	waitKey(0);
	return 0;
}
#elif defined TEST2		//用于显示不同参数的对比
int main()
{
	Mat src = imread("2.bmp");
	Mat mask = imread("2syk_1.jpg", 0);		//src mask像素不为零的区域为工件区域
	Mat fmask;			//前景掩码
	Mat bmask;			//背景掩码
	cvtColor(src, src, CV_BGR2HSV);
	threshold(mask, fmask, 254, 255, THRESH_BINARY_INV);	//Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted
	threshold(mask, bmask, 254, 255, THRESH_BINARY);	//Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted

	//imshow("fmask", fmask);
	imshow("bmask", bmask);

	calcIComponent(src, bmask);
	calcHSVComponent(src, bmask);

	drawData(Hbin, Hclass, "Bayesion/Hclass.txt");
	drawData(Sbin, Sclass, "Bayesion/Sclass.txt");
	drawData(Vbin, Vclass, "Bayesion/Vclass.txt");
	drawData(Ibin, Iclass, "Bayesion/Iclass.txt");

	waitKey(0);
	return 0;
}
#elif defined TEST3
int main()
{
	Configurations conf("my-conf.conf");
	Loggers::reconfigureLogger("default", conf);
	LOG(TRACE) << "Begin";

	Mat src = imread("3.bmp");
	Mat mask = imread("3syk.jpg", 0);		//src mask像素不为零的区域为工件区域
	Mat src1 = imread("2.bmp");
	Mat mask1 = imread("2syk.jpg", 0);
	cvtColor(src, src, CV_BGR2HSV);
	threshold(mask, mask, 254, 255, THRESH_BINARY_INV);	//Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted
	//imshow("mask", mask);
	cvtColor(src1, src1, CV_BGR2HSV);
	threshold(mask1, mask1, 254, 255, THRESH_BINARY_INV);	//Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted

	Ptr<NormalBayesClassifier> nbc = NormalBayesClassifier::create();

	/*获取训练样本*/
	Mat trainingData;			//训练样本
	Mat responseData;			//训练样本
	getTrainingData(src, trainingData);
	getResponseData(mask, responseData);
	//LOG(INFO) << responseData << endl;
	/**************/
	Ptr<TrainData> tData = TrainData::create(trainingData, ROW_SAMPLE, responseData);
	nbc->train(tData);

	/*第二次训练*/
	getTrainingData(src1, trainingData);
	getResponseData(mask1, responseData);
	//LOG(INFO) << responseData << endl;
	tData = TrainData::create(trainingData, ROW_SAMPLE, responseData);
	nbc->train(tData);


	Mat dst = imread("4.bmp");
	cvtColor(dst, dst, CV_BGR2HSV);

	Mat dstData;
	getTrainingData(dst, dstData);

	Mat result = Mat(dst.size(), CV_8UC1);
	getTrainResult(nbc, dstData, result);
	imshow("result", result);

	waitKey(0);
	return 0;
}
#endif