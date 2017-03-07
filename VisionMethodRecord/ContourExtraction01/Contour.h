#pragma once
#include <iostream>  
#include <vector>  
#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include "algorithm"
using namespace std;
using namespace cv;

#define PI 3.141592654;

//�������ڴ洢��Ӧ�����ĵ����С���������ĵ�
class Contour
{
public:
	Contour() {}
	void Initialize(vector<Point> cont);	
	void FetchPointPos(Point fetchpoint);						//����fetchpoint����Բ��Ͼ������������ϵ�е����λ��
	void FetchPointPosMapping(Contour src);		//���㽫src�е�fetchpoint�����λ��ӳ�䵽dst�У�����һ����ӳ��ԭʼ����ϵ�е����λ��

	vector<Point> contour;	//����
	double area;			//�������
	Vec4i hierachy;			//�����ȼ�

	Rect Box;				//������Χ����
	RotatedRect minBox;		//������С��Χ����
	//Point2f vertices[4];	//��Χ���εĶ���
	//Point2f center;			//��Χ���ε�����
	float radian_angle;		//���������Բ��ת�ĽǶȣ����ȣ�
	//int fetchpoint_dx;		//ץȡ��fetchpoint��������Բ�����������Բ�������ϵx�����ϵĲ�
	//int fetchpoint_dy;		//ץȡ��fetchpoint��������Բ�����������Բ�������ϵy�����ϵĲ�
	//int fetchpoint_dx_o;		//ץȡ��fetchpoint��������Բ���������ԭͼ������ϵx�����ϵĲ�
	//int fetchpoint_dy_o;		//ץȡ��fetchpoint��������Բ�����������ͼ������ϵy�����ϵĲ�
	Point2f kpoint_tran;		//fetchpoint transformed����Բ�������ϵ�е������ԲԲ��λ��
	Point2f kpoint_org;		//fetchpoint origin��ԭͼ������ϵ�е������ԲԲ��λ��
};

bool AreaCompare(Contour first, Contour second); //���ڱȽ�ǰ����������Ĵ�С