#ifndef _PROIMAGE_H_
#define _PROIMAGE_H_
#include "HSVHist.h"
#include <set>
using namespace std;
using namespace cv;
class ProImage:public HSVHist
{
public:
	ProImage();
	~ProImage();
	void preproImage();										//��ͼ�����Ԥ����
	void Init();
	void getContour();										//��ȡͼ��������ȥ������������
	void setImageWin(const string &_winname);				//�趨����������ʾ����
	void showImage();										//�����û����������ʾ����
	void fitContour();										//�����û�������������������������
	void writeResult(string _imgname = "");					//��ʾ�û����������ս��
	void coverImage(Mat &dst, Mat &img);					//��dstʹ��img���и���
	void getData(ProImage Temp);							//���ڽ�ģ�����ݴ�����dst

	static void onMouseHandle(int event, int x, int y, int flags, void* param);
	static int compArea(vector<Point> first, vector<Point> second);

	void reset();
private:
	Mat conindex;						//��¼��������ֵ
	vector<Vec4i> hierachy;				//�����ȼ�

	string winname;						//�û�������������
	Mat m_showimage;					//�û���������ͼ��
	vector<Mat> r_orgcon;				//�ֿ�洢ԭ����
	vector<Mat> b_orgcon;				//�ֿ�洢ԭ������������ʾ��

	int selectindex;					//��¼��ѡ��ĵ�����������
	int recoverflag;					//�Ƿ�ԭ�ı�ǣ���ʾ�ã�
	
protected:
	vector<vector<Point>> veccon;		//��¼ԭʼ����
	set<int> chosen;					//��¼��ѡ���������������
	vector<vector<Point>> vecpoly;		//��¼���ж������ϵ�����
	vector<RotatedRect> vececllipse;	//��¼������Բ��ϵ�����

};
#endif