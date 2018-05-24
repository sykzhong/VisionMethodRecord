#include "HSVHist.h"
#include <fstream>
#include <stdio.h>
#include <time.h>

INITIALIZE_EASYLOGGINGPP

class BoxBorder
{
public:
	BoxBorder();
	~BoxBorder();
	void getBoxAreaImage();		//经HSV色阶提取所得的盒子区域
	void getBoxContour();		//提取HSV色阶提取后的图像轮廓
	void getBoxCenter();		//以盒子区域中心作为盒子区域标志点
	void cannyBoxContour();		//利用canny算子提取盒子边缘
	void getBoxLine();			//利用霍夫直线变换
	void extractBoxArea();		//结合霍夫直线变换结果、盒子中心，将盒子区域提取出来
private:
	Point2f box_center;		//box center point flag

	Mat img_box;			//img only box
	Mat img_box_env;		//img with box and environment
	Mat img_box_area;		//box area extract by hsv
	Mat img_box_contour;	//box contour
	Mat img_box_canny;		//box canny src, used for canny

	//vector<Vec2f> box_border_line;	//houghline result
	vector<Vec4i> box_border_line;

};

BoxBorder::BoxBorder()
{
	string str_Box = ".\\BoxBorder\\0523_back.jpg", str_BoxEnv = ".\\BoxBorder\\0523_front_2.jpg";
	img_box = imread(str_Box);
	img_box_env = imread(str_BoxEnv);
	box_center = Point2f(0, 0);
}

BoxBorder::~BoxBorder()
{

}

void BoxBorder::getBoxAreaImage()
{
	string str_Box = ".\\BoxBorder\\0523_back.jpg", str_BoxEnv = ".\\BoxBorder\\0523_front_2.jpg";
	HSVHist img_Box, img_BoxEnv;

	img_Box.getImage(str_Box);
	img_BoxEnv.getImage(str_BoxEnv);

	img_Box.preprocess();
	img_BoxEnv.preprocess();

	HSVHist::removeBack(img_BoxEnv, img_Box);
	img_BoxEnv.showImage();			//result img name: "houghtest.jpg"
	img_box_area = imread("box_area.jpg");

}

void BoxBorder::getBoxCenter()
{
	Mat gray_box_area = Mat::zeros(img_box_area.size(), CV_8UC1);
	cvtColor(img_box_area, gray_box_area, CV_RGB2GRAY);
	int nRows = gray_box_area.rows;
	int nCols = gray_box_area.cols;
	int nChannels = gray_box_area.channels();

	if (nChannels != 1)
	{
		cerr << "Error: gray_box_area channel != 1.";
		return;
	}
	uchar *p_src;			//img_box_area行指针
	int points_cnt = 0;
	
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			//提取原图中的色阶
			p_src = gray_box_area.ptr<uchar>(i);
			if (p_src[j] != 0)
			{
				box_center.x += i;
				box_center.y += j;
				points_cnt++;
			}
		}
	}
	box_center.x /= points_cnt;
	box_center.y /= points_cnt;
	Mat tmp_img = img_box_env.clone();
	circle(tmp_img, box_center, 10, RED, -1);
	imwrite("box_center.jpg", tmp_img);
}

void BoxBorder::getBoxContour()
{
	Mat conimage;
	cvtColor(img_box_area, conimage, CV_BGR2GRAY);

	int morph_elem = 1;				//Element:\n 0: Rect - 1: Cross - 2: Ellipse
	int morph_size = 1;
	int morph_operation = 0;		//0: Opening - 1: Closing \n 2: Gradient - 3: Top Hat \n 4: Black Hat
	cv::Mat element = cv::getStructuringElement(morph_elem, cv::Size(2 * morph_size + 1, 2 * morph_size + 1), cv::Point(morph_size, morph_size));
	cv::Mat opening;
	//cv::morphologyEx(threshed_image, opening, morph_operation, element, cv::Point(-1, -1), 2);
	cv::erode(conimage, conimage, element);

	vector<vector<Point>> veccon;		//记录原始轮廓
	vector<Vec4i> hierachy;				//轮廓等级
	findContours(conimage, veccon, hierachy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<int> index;

	for (int i = 0; i < veccon.size(); i++)		//去除细小轮廓
		if (contourArea(veccon[i]) < img_box_area.cols*img_box_area.rows / 50)
		{
			veccon.erase(veccon.begin() + i);
			i--;
		}

	/// Draw contours
	img_box_contour = Mat::zeros(conimage.size(), CV_8UC1);
	for (int i = 0; i< veccon.size(); i++)
	{
		Scalar color = WHITE;
		drawContours(img_box_contour, veccon, i, color, 1, 8, hierachy, 0, Point());
	}

	/// Show in a window
	//imshow("Contours", img_box_contour);
	imwrite("box_contour.jpg", img_box_contour);
}

void BoxBorder::cannyBoxContour()
{
	Mat canny_mask;
	int morph_elem = 2;				//Element:\n 0: Rect - 1: Cross - 2: Ellipse
	int morph_size = 10;
	int morph_operation = 0;		//0: Opening - 1: Closing \n 2: Gradient - 3: Top Hat \n 4: Black Hat
	cv::Mat element = cv::getStructuringElement(morph_elem, cv::Size(2 * morph_size + 1, 2 * morph_size + 1), cv::Point(morph_size, morph_size));
	cv::Mat opening;
	//cv::morphologyEx(threshed_image, opening, morph_operation, element, cv::Point(-1, -1), 2);
	cv::dilate(img_box_contour, canny_mask, element);
	imwrite("box_canny_mask.jpg", canny_mask);
	
	Mat canny_src = img_box_env.clone();
	GaussianBlur(canny_src, canny_src, Size(51, 51), 0);
	
	int nRows = canny_src.rows;
	int nCols = canny_src.cols;
	int nChannels = canny_src.channels();

	if (nRows != img_box_env.rows || nCols != img_box_env.cols || nChannels != img_box_env.channels())
	{
		cerr << "Error: canny_src not match img_box_env";
		return;
	}

	if (canny_src.isContinuous() == true)
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *p_canny_src;		//用于canny的经模糊的图片
	uchar *p_canny_mask;	//canny_mask
	uchar *p_src;			//原图片

	Mat blured_box_env = Mat::zeros(img_box_env.size(), CV_8UC3);
	medianBlur(img_box_env, blured_box_env, 3);
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			//提取原图中的色阶
			p_canny_src = canny_src.ptr<uchar>(i);
			p_canny_mask = canny_mask.ptr<uchar>(i);
			p_src = blured_box_env.ptr<uchar>(i);
			if (p_canny_mask[j] != 0)
			{
				for (int k = 0; k < nChannels; k++)
					p_canny_src[j*nChannels + k] = p_src[j*nChannels + k];
			}
		}
	}
	//imshow("canny_src", canny_src);
	imwrite("box_canny_src.jpg", canny_src);

	img_box_canny = Mat::zeros(canny_src.size(), CV_8UC1);
	cvtColor(canny_src, img_box_canny, CV_BGR2GRAY);
	const int lowThreshold = 50;
	const int highThreshold = 200;
	const int kernel_size = 3;
	Canny(img_box_canny, img_box_canny, lowThreshold, highThreshold, kernel_size);
	//imshow("box_canny.jpg", img_box_canny);
	imwrite("box_canny.jpg", img_box_canny);
}

void BoxBorder::getBoxLine()
{
	
	Mat img_box_border = img_box_env.clone();

	//HoughLines(img_box_canny, box_border_line, 2, CV_PI / 180, 300, 0, 0);
	//for (size_t i = 0; i < box_border_line.size(); i++)
	//{
	//	float rho = box_border_line[i][0], theta = box_border_line[i][1];
	//	Point pt1, pt2;
	//	double a = cos(theta), b = sin(theta);
	//	double x0 = a*rho, y0 = b*rho;
	//	pt1.x = cvRound(x0 + 1000 * (-b));
	//	pt1.y = cvRound(y0 + 1000 * (a));
	//	pt2.x = cvRound(x0 - 1000 * (-b));
	//	pt2.y = cvRound(y0 - 1000 * (a));
	//	line(img_box_border, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
	//}

	
	//HoughLinesP(img_box_canny, box_border_line, 1, CV_PI / 180, 200, 100, 200);			//使用canny提取轮廓进行霍夫变换
	HoughLinesP(img_box_contour, box_border_line, 1, CV_PI / 180, 200, 100, 200);			//使用HSV色阶提取轮廓进行霍夫变换
	for (size_t i = 0; i < box_border_line.size(); i++)
	{
		Vec4i l = box_border_line[i];
		line(img_box_border, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}
	imshow("detected box_border_line", img_box_border);
	imwrite("box_border.jpg", img_box_border);
}

void BoxBorder::extractBoxArea()
{
	Mat result = img_box_env.clone();

	vector<int> border_flag(box_border_line.size());								//用于记录box_center在各line的位置
	vector<vector<double> > line_param(box_border_line.size(), vector<double>(3));	//用于记录各直线参数
	
	for (int i = 0; i < box_border_line.size(); i++)
	{
		Vec4i l = box_border_line[i];
		Point2f p1 = Point2f(l[0], l[1]);
		Point2f p2 = Point2f(l[2], l[3]);
		line_param[i][0] = p2.y - p1.y;
		line_param[i][1] = p1.x - p2.x;
		line_param[i][2] = p2.x*p1.y - p1.x*p2.y;

		double A = line_param[i][0];
		double B = line_param[i][1];
		double C = line_param[i][2];
		if (A*box_center.x + B*box_center.y + C > 0)
			border_flag[i] = 1;
		else
			border_flag[i] = -1;
	}

	int nRows = result.rows;
	int nCols = result.cols;
	int nChannels = result.channels();

	uchar *p_src;			//result行指针

	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			p_src = result.ptr<uchar>(i);

			for (int k = 0; k < box_border_line.size(); k++)
			{
				double A = line_param[k][0];
				double B = line_param[k][1];
				double C = line_param[k][2];
				if ((A*j + B*i + C) * border_flag[k] < 0)
				{
					for (int n = 0; n < nChannels; n++)
						p_src[j*nChannels + n] = 0;
				}
			}
		}
	}
	imwrite("result.jpg", result);
}

int main()
{
	Configurations conf("my-conf.conf");
	Loggers::reconfigureLogger("default", conf);
	LOG(TRACE) << "Begin";

	BoxBorder boxborder;
	boxborder.getBoxAreaImage();
	boxborder.getBoxCenter();
	boxborder.getBoxContour();
	//boxborder.cannyBoxContour();
	boxborder.getBoxLine();
	boxborder.extractBoxArea();
	waitKey(0);
	return 0;
}
