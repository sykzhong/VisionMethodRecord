// ContourExtraction02.cpp : �������̨Ӧ�ó������ڵ㡣
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

//string strSrc = "E:/Cloud/Research/Vision/Picture/gzt_v3/gzt01_shrinked_2_filtered.jpg";   //����ͼ���ַ��
//string strTemplate= "E:/Cloud/Research/Vision/Picture/gzt_v3/template_filtered.jpg";    //����ƥ���ģ���ַ��

//string strSrc = "E:/Cloud/Research/Vision/Picture/workpiece_v1/workpiece01.jpg";   //����ͼ���ַ��
//string strTemplate= "E:/Cloud/Research/Vision/Picture/workpiece_v1/workpiece01_template.jpg";    //����ƥ���ģ���ַ��

string strSrc = "E:/Cloud/Research/Vision/Picture/workpiece_v3/Src_filtered.jpg";   //����ͼ���ַ��
string strTemplate = "E:/Cloud/Research/Vision/Picture/workpiece_v3/Template_filtered.jpg";    //����ƥ���ģ���ַ��

//string strSrc = "E:/Cloud/Research/Vision/Picture/workpiece_v4/Src_3(shrinked)_filtered.jpg";   //����ͼ���ַ��
//string strTemplate = "E:/Cloud/Research/Vision/Picture/workpiece_v4/Template_3(shrinked)_filtered.jpg";    //����ƥ���ģ���ַ��

int th = 110;  //���ڶ�ͼ����ж�ֵ������С��ֵ���������ƿ׹���
//int th = 25;  //���ڶ�ͼ����ж�ֵ������С��ֵ��������˨����

int main()
{
	Mat Src = imread(strSrc, CV_LOAD_IMAGE_GRAYSCALE);				//�ɼ�����ԭͼ��
	Mat Template = imread(strTemplate, CV_LOAD_IMAGE_GRAYSCALE);	//����ƥ���ģ��ͼ��
	if (!Src.data || !Template.data)
		return 0;

	Mat Template_con = Template.clone();		//����ģ��ͼ����������ʾ
	Mat Src_con = Src.clone();					//����ԭͼ����������ʾ
	cvtColor(Template_con, Template_con, CV_GRAY2BGR);	
	cvtColor(Src_con, Src_con, CV_GRAY2BGR);

	vector<vector<Point>> contours_Src, contours_Template;		//���ڴ洢ԭͼ���ģ��ͼ�����������
	vector<Vec4i> hierarchy_Src, hierarchy_Template;

	Mat Src_th, Template_th;				//���ڴ洢ԭͼ��ģ��ͼ���ֵ�����ͼ��
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

	/////////////ԭͼ��ģ��ͼ��������ȡ///////////
	findContours(Src_th, contours_Src, hierarchy_Src, 
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	findContours(Template_th, contours_Template, hierarchy_Template, 
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	/////////////////////////////////

	/////////////ģ��ͼ�񣺹���������ȡ///////////
	/*˼�����ɷ��ٲ�����������ʵ���������������
	�����ʹ���������¼����Ϸ������ж��������Ĵ洢����*/
	vector<Contour> Tconvec;		//Template contour vector ģ��ͼ����������
	Contour WorkPiececonT;			//workpicec contour of Template ģ��ͼ���еĹ�������
	Contour temp;
	for (int i = 0; i < contours_Template.size(); i++)	//ģ��ͼ�������������г�ʼ��
	{
		temp.Initialize(contours_Template[i]);
		Tconvec.push_back(temp);
	}
	sort(Tconvec.begin(), Tconvec.end(), AreaCompare);		//ģ��ͼ�������������������С����
	WorkPiececonT = *(Tconvec.begin() + 1);
	///////////////////////////////////////

	////////////ģ��ͼ�񣺹���ץȡ�㶨λ//////////
	Point Tfetchpoint;			//template key pointģ��ͼ��ץȡ��
	//Tfetchpoint = WorkPiececonT.minBox.center;						//�û��Զ���ץȡ��λ��
	//Tfetchpoint = Point(Template_th.cols / 2, Template.rows / 2); 	//�û��Զ���ץȡ��λ�ã�������Ϊtemplateͼ�������
	Tfetchpoint = Point(4, 4);
	WorkPiececonT.FetchPointPos(Tfetchpoint);						//���û��Զ���ץȡ��λ�ý��д洢
	//cout << "the keypointpos(origin) of workPiececonT is " << Tfetchpoint << endl;
	vector<vector<Point>> tempcon;									//syk���ڽ�vector<point>ת��Ϊvector<vector<Point>>������Ӧdrawcontours
	tempcon.clear();												//���£���ģ��ͼ���������ͼ����ʾ����
	tempcon.push_back(WorkPiececonT.contour);
	drawContours(Template_con, tempcon, -1, Scalar(0, 0, 255), 3, 8);
	ellipse(Template_con, WorkPiececonT.minBox, Scalar(0, 255, 0), 2, 8);
	circle(Template_con, Tfetchpoint, 3, Scalar(255, 0, 0), -1, 8);
	//////////////////////////

	//////////ģ��ͼ�񣺹�����������ʾ//////////
	imwrite("Template_con.jpg", Template_con);
	namedWindow("Template_con", CV_WINDOW_NORMAL);
	imshow("Template_con", Template_con);
	/////////////////////////////////////////

	////////////ԭͼ�񣺹�������ƥ��///////////
	vector<Contour> WorkPiececonS;	//Src contour of Template ԭͼ���еĹ�������
	for (int i = 0; i < contours_Src.size(); i++)	//ԭͼ�������������г�ʼ�������������С�ж������Ƿ����Թ���
	{
		temp.Initialize(contours_Src[i]);
		if (temp.area >= 0.8*WorkPiececonT.area && temp.area <= 1.2*WorkPiececonT.area)
		{
			temp.FetchPointPosMapping(WorkPiececonT);				//���ڽ�ģ��ͼ���е�ץȡ��ӳ�䵽ԭͼ����
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

	//////////ԭͼ�񣺹�����������ʾ//////////
	imwrite("Src_con.jpg", Src_con);
	namedWindow("Src_con", CV_WINDOW_NORMAL);
	imshow("Src_con", Src_con);
	waitKey(0);
	/////////////////////////////////////////

    return 0;
}

