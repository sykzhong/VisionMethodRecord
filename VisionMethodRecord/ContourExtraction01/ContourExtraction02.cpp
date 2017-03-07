// ContourExtraction02.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>  
#include <vector>  
#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include "Contour.h"
using namespace std;
using namespace cv;

//string strSrc = "E:/Cloud/Research/Vision/Picture/gzt_v3/gzt01_shrinked_2_filtered.jpg";   //整体图像地址名
//string strTemplate= "E:/Cloud/Research/Vision/Picture/gzt_v3/template_filtered.jpg";    //用于匹配的模板地址名

//string strSrc = "E:/Cloud/Research/Vision/Picture/workpiece_v1/workpiece01.jpg";   //整体图像地址名
//string strTemplate= "E:/Cloud/Research/Vision/Picture/workpiece_v1/workpiece01_template.jpg";    //用于匹配的模板地址名

string strSrc = "E:/Cloud/Research/Vision/Picture/workpiece_v3/Src_filtered.jpg";   //整体图像地址名
string strTemplate = "E:/Cloud/Research/Vision/Picture/workpiece_v3/Template_filtered.jpg";    //用于匹配的模板地址名

//string strSrc = "E:/Cloud/Research/Vision/Picture/workpiece_v4/Src_3(shrinked)_filtered.jpg";   //整体图像地址名
//string strTemplate = "E:/Cloud/Research/Vision/Picture/workpiece_v4/Template_3(shrinked)_filtered.jpg";    //用于匹配的模板地址名

int th = 110;  //用于对图像进行二值化的最小阈值，用于螺纹孔工件
//int th = 25;  //用于对图像进行二值化的最小阈值，用于螺栓工件

int main()
{
	Mat Src = imread(strSrc, CV_LOAD_IMAGE_GRAYSCALE);				//采集所得原图像
	Mat Template = imread(strTemplate, CV_LOAD_IMAGE_GRAYSCALE);	//用于匹配的模板图像
	if (!Src.data || !Template.data)
		return 0;

	Mat Template_con = Template.clone();		//用于模板图像轮廓的显示
	Mat Src_con = Src.clone();					//用于原图像轮廓的显示
	cvtColor(Template_con, Template_con, CV_GRAY2BGR);	
	cvtColor(Src_con, Src_con, CV_GRAY2BGR);

	vector<vector<Point>> contours_Src, contours_Template;		//用于存储原图像和模板图像的所有轮廓
	vector<Vec4i> hierarchy_Src, hierarchy_Template;

	Mat Src_th, Template_th;				//用于存储原图像、模板图像二值化后的图像
	//threshold(Src, Src_th, th, 255, CV_THRESH_BINARY);
	//threshold(Template, Template_th, th, 255, CV_THRESH_BINARY);

	threshold(Src, Src_th, th, 255, CV_THRESH_BINARY_INV);
	threshold(Template, Template_th, th, 255, CV_THRESH_BINARY_INV);
	//equalizeHist(Src, Src);
	//equalizeHist(Template, Template);
	//blur(Template, Template, Size(5, 5));
	//blur(Src, Src, Size(5, 5));
	//Canny(Src, Src_th, th, 3 * th, 3);
	//Canny(Template, Template_th, th, 3 * th, 3);

	namedWindow("Template threshold", CV_WINDOW_NORMAL);
	imshow("Template threshold", Template_th);
	namedWindow("Src threshold", CV_WINDOW_NORMAL);
	imshow("Src threshold", Src_th);

	/////////////原图像、模板图像轮廓提取///////////
	findContours(Src_th, contours_Src, hierarchy_Src, 
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	findContours(Template_th, contours_Template, hierarchy_Template, 
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	/////////////////////////////////

	/////////////模板图像：工件轮廓提取///////////
	/*思考：可否再不用类的情况下实现轮廓按面积排序；
	亦或在使用类的情况下减少上方代码中对于轮廓的存储步骤*/
	vector<Contour> Tconvec;		//Template contour vector 模板图像所有轮廓
	Contour WorkPiececonT;			//workpicec contour of Template 模板图像中的工件轮廓
	Contour temp;
	for (int i = 0; i < contours_Template.size(); i++)	//模板图像所有轮廓序列初始化
	{
		temp.Initialize(contours_Template[i]);
		Tconvec.push_back(temp);
	}
	sort(Tconvec.begin(), Tconvec.end(), AreaCompare);		//模板图像中轮廓按轮廓面积大小排序
	WorkPiececonT = *(Tconvec.begin() + 1);
	///////////////////////////////////////

	////////////模板图像：工件抓取点定位//////////
	Point Tfetchpoint;			//template key point模板图像抓取点
	//Tfetchpoint = WorkPiececonT.minBox.center;						//用户自定义抓取点位置
	//Tfetchpoint = Point(Template_th.cols / 2, Template.rows / 2); 	//用户自定义抓取点位置，这里设为template图像的中心
	Tfetchpoint = Point(4, 4);
	WorkPiececonT.FetchPointPos(Tfetchpoint);						//将用户自定义抓取点位置进行存储
	//cout << "the keypointpos(origin) of workPiececonT is " << Tfetchpoint << endl;
	vector<vector<Point>> tempcon;									//syk用于将vector<point>转换为vector<vector<Point>>，以适应drawcontours
	tempcon.clear();												//以下，将模板图像的轮廓在图中显示出来
	tempcon.push_back(WorkPiececonT.contour);
	drawContours(Template_con, tempcon, -1, Scalar(0, 0, 255), 3, 8);
	ellipse(Template_con, WorkPiececonT.minBox, Scalar(0, 255, 0), 2, 8);
	circle(Template_con, Tfetchpoint, 3, Scalar(255, 0, 0), -1, 8);
	//////////////////////////

	//////////模板图像：工件轮廓的显示//////////
	imwrite("Template_con.jpg", Template_con);
	namedWindow("Template_con", CV_WINDOW_NORMAL);
	imshow("Template_con", Template_con);
	/////////////////////////////////////////

	////////////原图像：工件轮廓匹配///////////
	vector<Contour> WorkPiececonS;	//Src contour of Template 原图像中的工件轮廓
	for (int i = 0; i < contours_Src.size(); i++)	//原图像所有轮廓序列初始化，并用面积大小判断轮廓是否来自工件
	{
		temp.Initialize(contours_Src[i]);
		if (temp.area >= 0.8*WorkPiececonT.area && temp.area <= 1.2*WorkPiececonT.area)
		{
			temp.FetchPointPosMapping(WorkPiececonT);				//用于将模板图像中的抓取点映射到原图像中
			WorkPiececonS.push_back(temp);
		}
	}
	
	for (vector<Contour>::iterator iter = WorkPiececonS.begin(); iter != WorkPiececonS.end(); iter++)
	{
		tempcon.clear();
		tempcon.push_back(iter->contour);
		drawContours(Src_con, tempcon, -1, Scalar(0, 0, 255), 3, 8);
		cout << "The center of ellipse is "<< iter->minBox.center << ", the keypoint is " 
			<< iter->kpoint_org + iter->minBox.center << endl;
		//for (int i = 0; i < 4; i++)
		//{
		//	line(Src_con, iter->vertices[i], iter->vertices[(i + 1) % 4], Scalar(0, 255, 0), 2, 8, 0);
		//}
		ellipse(Src_con, iter->minBox, Scalar(0, 255, 0), 2, 8);
		circle(Src_con, iter->kpoint_org + iter->minBox.center, 3, Scalar(255, 0, 0), -1, 8);
	}
	/////////////////////////////////////////

	//////////原图像：工件轮廓的显示//////////
	imwrite("Src_con.jpg", Src_con);
	namedWindow("Src_con", CV_WINDOW_NORMAL);
	imshow("Src_con", Src_con);
	waitKey(0);
	/////////////////////////////////////////

    return 0;
}

