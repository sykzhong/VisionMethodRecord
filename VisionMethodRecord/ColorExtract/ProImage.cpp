#include "stdafx.h"
#include "ProImage.h"
ProImage::ProImage()
{
	
}

ProImage::~ProImage()
{

}

void ProImage::Init()
{
	veccon.clear();
	chosen.clear();
	reset();
}

void ProImage::reset()
{
	selectindex = -1;
	recoverflag = 0;
}

void ProImage::preproImage()
{
	medianBlur(m_image, m_image, 5);

	int morph_elem = 0;		//Element:\n 0: Rect - 1: Cross - 2: Ellipse
	int morph_size = 5;
	Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
	//dilate(m_image, m_image, element, Point(-1, -1), 1);
	//erode(m_image, m_image, element, Point(-1, -1), 1);
	
	//morphologyEx(m_image, m_image, MORPH_OPEN, element);
	imshow("Proimage", m_image);
}

void ProImage::getContour()
{
	this->preproImage();
	Mat conimage;
	cvtColor(m_image, conimage, CV_BGR2GRAY);
	conindex = Mat(m_image.rows, m_image.cols, CV_8UC1, Scalar::all(0));
	findContours(conimage, veccon, hierachy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<int> index;
	for (int i = 0; i < veccon.size(); i++)
		if (contourArea(veccon[i]) < m_image.cols*m_image.rows / 50)
		{
			veccon.erase(veccon.begin() + i);
			i--;
		}
	sort(veccon.begin(), veccon.end(), ProImage::compArea);
	r_orgcon.resize(veccon.size());
	b_orgcon.resize(veccon.size());
	////////////////在conindex中记录轮廓坐标及索引值
	for (int i = 0; i < veccon.size(); i++)
	{
		drawContours(conindex, veccon, i, Scalar(i + 1), 5);
		r_orgcon[i] = Mat(m_image.size(), CV_8UC3, Scalar::all(0));
		b_orgcon[i] = Mat(m_image.size(), CV_8UC3, Scalar::all(0));
		drawContours(r_orgcon[i], veccon, i, RED, 2);
		drawContours(b_orgcon[i], veccon, i, BLUE, 2);
	}

}

void ProImage::setImageWin(const string &_winname)
{
	if (_winname.empty())
		return;
	winname = _winname;
	
	chosen.clear();
	//////初始化界面图像，将轮廓标红////////
	m_showimage = srcimage.clone();
	for (int i = 0; i < veccon.size(); i++)
		coverImage(m_showimage, r_orgcon[i]);
	reset();
	namedWindow(winname, WINDOW_AUTOSIZE);
	///////////////////////////////////////
}

void ProImage::coverImage(Mat &dst, Mat &img)
{
	CV_Assert(dst.size() == img.size());
	int nChannels = img.channels();
	int nRows = img.rows;
	int nCols = img.cols;
	if (img.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar* p;
	uchar* q;
	for(int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
		{
			p = img.ptr<uchar>(i);
			q = dst.ptr<uchar>(i);
			int k = 0;
			for (k = 0; k < nChannels; k++)
				if (p[j*nChannels+k] != 0)
					break;
			if (k < 3)
				for (k = 0; k < nChannels; k++)
					q[j*nChannels + k] = p[j*nChannels + k];					
		}
}

void ProImage::showImage()
{
	vector<Mat>::iterator iter;
	if (recoverflag == 1)							//轮廓由蓝色（高亮）变为红色（普通）
		iter = r_orgcon.begin();
	else if(recoverflag == 0)
		iter = b_orgcon.begin();
	
	if (recoverflag = 1 && 
		chosen.find(selectindex) != chosen.end())	//防止鼠标滑动过程将已选定轮廓复原为红色
		return;

	for (int i = 0; i < veccon.size(); i++)
		if (i == selectindex)
			coverImage(m_showimage, *(iter+i));

	imshow(winname, m_showimage);
}

void ProImage::fitContour()
{
	vecpoly.resize(chosen.size());
	vececllipse.resize(veccon.size() - vecpoly.size());
	int j = 0, k = 0;
	for (int i = 0; i < veccon.size(); i++)
	{
		if (chosen.find(i) != chosen.end())
		{
			approxPolyDP(veccon[i], vecpoly[j], 10, true);
			j++;
		}
		else
		{
			Mat pointsf;
			Mat(veccon[i]).convertTo(pointsf, CV_32F);
			vececllipse[k] = fitEllipse(pointsf);
			k++;
		}
	}
}

void ProImage::writeResult(string _imgname)
{
	Mat result = srcimage.clone();
	for (int i = 0; i < vecpoly.size(); i++)
		drawContours(result, vecpoly, i, RED, 2);
	for(int i = 0; i < vececllipse.size(); i++)
		ellipse(result, vececllipse[i], RED, 2, CV_AA);
	if (_imgname == "")
		_imgname = "result.jpg";
	//imshow(_imgname, result);
	imwrite(_imgname, result);
}

int ProImage::compArea(vector<Point> first, vector<Point> second)
{
	return contourArea(first) > contourArea(second);
}

void ProImage::onMouseHandle(int event, int x, int y, int flags, void* param)
{
	ProImage& proimage = *(ProImage*)param;
	int index = (int)proimage.conindex.at<uchar>(y, x) - 1;		//轮廓索引值，负值表示无轮廓
	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		if (index >= 0)
		{
			if (proimage.chosen.find(index) != proimage.chosen.end())
				proimage.chosen.erase(index);
			else
				proimage.chosen.insert(index);
			proimage.showImage();
		}
	case CV_EVENT_MOUSEMOVE:
		if (index >= 0)
		{
			if (proimage.selectindex == -1)
			{
				proimage.selectindex = index;
				proimage.recoverflag = 0;
				proimage.showImage();
			}
		}
		else
		{
			if (proimage.selectindex != -1)
			{
				proimage.recoverflag = 1;
				proimage.showImage();
				proimage.selectindex = -1;
			}
		}
		break;
	}
}

void ProImage::getData(ProImage Temp)
{
	set<int>::iterator iter;
	for (iter = Temp.chosen.begin(); iter != Temp.chosen.end(); iter++)
		chosen.insert(*iter);
}