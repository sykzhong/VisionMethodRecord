#ifndef _HSVHIST_H_
#define _HSVHIST_H_
#include "GlobalHeader.h"

class HSVHist
{
public:
	HSVHist();
	~HSVHist();
	int getImage(string path);
	void getHist(Mat &mask = Mat());
	void drawHist();
	void removeSeg(HSVHist back);							//ȥ������ͬɫ�׵Ĳ���
	void preprocess();										//����ɫ����ȡǰ��ͼ��Ԥ����
	static void removeBack(HSVHist &dst, HSVHist &back);
	void showImage(string strpath = "");
	void Init();
	void Split(const Mat& src, Mat* mv, Mat &mask);			//�������split�������ֽ����õ�ͨ��mat����Ϊ1�����ڱ���ͼ��

	void getTargetContour();
private:
	Mat hsvplane[3];		//�洢HSV��ͨ��
	
	float svrange[2];
	float hrange[2];
	const float *ranges[3];	//hsv��ȡֵ��Χ
	double maxval;			//ԭʼ�����е����ֵ

	int bin_w;				//ֱ��ͼ������
	int hist_h, hist_w;		//ֱ��ͼ�����
	Mat hist_img;			//������ʾ������ֱ��ͼ

protected:
	string path;			//ԭͼ·��

	Mat srcimage;			//ԭͼ��,��ȡΪHSV��ʽ
	Mat m_image;
	MatND hsvhist;			//���ڴ洢ֱ��ͼ������
	int histsize[3];		//hsv��ֱ��ͼ�зֲ���

};


#endif

