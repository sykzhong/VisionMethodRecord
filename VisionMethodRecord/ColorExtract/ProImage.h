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
	void preproImage();										//对图像进行预处理
	void Init();
	void getContour();										//提取图像轮廓并去除不合理轮廓
	void setImageWin(const string &_winname);				//设定操作界面显示窗口
	void showImage();										//根据用户操作情况显示界面
	void fitContour();										//根据用户操作情况对轮廓进行重新拟合
	void writeResult(string _imgname = "");					//显示用户操作的最终结果
	void coverImage(Mat &dst, Mat &img);					//对dst使用img进行覆盖
	void getData(ProImage Temp);							//用于将模板数据传递至dst

	static void onMouseHandle(int event, int x, int y, int flags, void* param);
	static int compArea(vector<Point> first, vector<Point> second);

	void reset();
private:
	Mat conindex;						//记录轮廓索引值
	vector<Vec4i> hierachy;				//轮廓等级

	string winname;						//用户操作窗口名称
	Mat m_showimage;					//用户操作界面图像
	vector<Mat> r_orgcon;				//分块存储原轮廓
	vector<Mat> b_orgcon;				//分块存储原轮廓（高亮显示）

	int selectindex;					//记录被选择的单个轮廓索引
	int recoverflag;					//是否复原的标记（显示用）
	
protected:
	vector<vector<Point>> veccon;		//记录原始轮廓
	set<int> chosen;					//记录被选择的所有轮廓索引
	vector<vector<Point>> vecpoly;		//记录进行多边形拟合的轮廓
	vector<RotatedRect> vececllipse;	//记录进行椭圆拟合的轮廓

};
#endif