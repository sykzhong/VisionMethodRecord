#include "HSVHist.h"
HSVHist::HSVHist()
{
	histsize[0] = 8;		//H通道bin数
	histsize[1] = 16;		//S通道
	histsize[2] = 8;		//V通道

	hrange[0] = 0;			//神tmbug，不如此中规中矩地设为180会出错
	hrange[1] = 180;
	ranges[0] = hrange;

	svrange[0] = 0;
	svrange[1] = 255;
	for (int i = 1; i < 3; i++)
		ranges[i] = svrange;		//各通道取值范围
	
	bin_w = 3;
	hist_h = 240;
	hist_w = histsize[0]*histsize[1] *bin_w;	//hs的bin数总和，单格宽为6
	hist_img = Mat(hist_h, hist_w, CV_8UC3, Scalar::all(0));
}

HSVHist::~HSVHist()
{

}

void HSVHist::Split(const Mat& src, Mat* mv, Mat &mask)
{
	int nChannels = src.channels();
	int nRows = src.rows;
	int nCols = src.cols;
	uchar **splitChannels = new uchar*[3];
	//vector<vector<uchar>> splitChannels(nChannels);
	//if (src.isContinuous())
	//{
	//	nCols *= nRows;
	//	nRows = 1;
	//}
	const uchar *p;		//指代src的行指针
	uchar *q;			//指代mask的行指针
	/*用于存储HSV三通道的值*/
	uchar *data1 = new uchar[nRows*nCols];
	uchar *data2 = new uchar[nRows*nCols];
	uchar *data3 = new uchar[nRows*nCols];
	//int *data1 = new int[nRows*nCols];
	//int *data2 = new int[nRows*nCols];
	//int *data3 = new int[nRows*nCols];
	int index = 0;		//用于记录三通道的末下标

	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			p = src.ptr<uchar>(i);
			q = mask.ptr<uchar>(i);
			//LOG(TRACE) << (int)(q[j*nChannels]);
			if ((int)(q[j*nChannels]) > 10)
			{
				data1[index] = p[j*nChannels + 0];
				data2[index] = p[j*nChannels + 1];
				data3[index] = p[j*nChannels + 2];
				//cout << (int)data2[index] << endl;
				index++;
			}
			/*for (k = 0; k < nChannels; k++)
				splitChannels[k].push_back(p[j*nChannels + k]);*/
		}

	}
	cout << index << endl;
	mv[0] = Mat(1, index, CV_8UC1, data1);
	mv[1] = Mat(1, index, CV_8UC1, data2);
	mv[2] = Mat(1, index, CV_8UC1, data3);
	LOG(TRACE) << mv[0];

	//for (k = 0; k < nChannels; k++)
	//	mv[k] = Mat(1, splitChannels[k].size(), CV_8UC1);
}

int HSVHist::getImage(string path)
{
	srcimage = imread(path, 1);
	if (!srcimage.data)
	{
		cerr << "Error: can't read the image" << endl;
		return 0;
	}
	this->path = path;
	m_image = srcimage.clone();
	cvtColor(m_image, m_image, COLOR_BGR2HSV);
	//medianBlur(m_image, m_image, 3);
	return 1;
}

void HSVHist::preprocess()
{
	medianBlur(m_image, m_image, 3);
}

void HSVHist::Init()
{
	getHist();
	//drawHist();
}

void HSVHist::getHist(Mat &mask)
{
	int channels[3] = { 0, 1, 2 };
	split(m_image, hsvplane);
	calcHist(hsvplane, 2, channels, mask, hsvhist, 2, histsize, ranges);
		
	//归一化处理
	minMaxLoc(hsvhist, 0, &maxval, 0, 0);
	for (int i = 0; i < histsize[0]; i++)
		for (int j = 0; j < histsize[1]; j++)
		{
			float binval = hsvhist.at<float>(i, j);
			binval = round(binval*hist_h / maxval);
			hsvhist.at<float>(i, j) = binval;
		}
}

void HSVHist::drawHist()
{
	Mat hsv_color = Mat(1, 1, CV_8UC3);
	Mat rgb_color = Mat(1, 1, CV_8UC3);
	for(int i = 0; i < histsize[0]; i++)
		for (int j = 0; j < histsize[1]; j++)
		{
			int loc = i*histsize[1] + j;
			float binval = hsvhist.at<float>(i, j);
			//printf("i:%d j:%d binval:%.0f\n", i, j, binval);
			
			//颜色由hsv转换为rgb
			hsv_color.setTo(Scalar(i*179.f / histsize[0], j*255.f / histsize[1], 255, 0));
			//hsv_color.setTo(Scalar(i*179.f / histsize[0], 255, j*255.f / histsize[1], 0));		//HV版本
			cvtColor(hsv_color, rgb_color, COLOR_HSV2BGR);
			Scalar color = rgb_color.at<Vec3b>(0, 0);
			
			rectangle(hist_img, Point(loc*bin_w, hist_h), Point((loc + 1)*bin_w, hist_h - binval), color, -1);
		}
	imshow(path, hist_img);
}

void HSVHist::removeSeg(HSVHist back)
{
	int nRows = m_image.rows;
	int nCols = m_image.cols;
	int nChannels = m_image.channels();
	if(m_image.isContinuous() == true)
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar *psrc;
	int Hval, Sval;
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			//提取原图中的色阶
			psrc = m_image.ptr<uchar>(i);
			Hval = psrc[j*nChannels] * histsize[0] / 180;
			Sval = psrc[j*nChannels + 1] * histsize[1] / 256;

			if (back.hsvhist.at<float>(Hval, Sval) < 2)
				for (int k = 0; k < 3; k++)
					psrc[j*nChannels + k] = 0;
		}
	}


	cvtColor(m_image, m_image, CV_HSV2BGR);
}

void HSVHist::showImage(string strpath)
{
	if (strpath == "")
		strpath = this->path;
	strpath += "hist";
	//imshow(strpath, m_image);
	imwrite("box_area.jpg", m_image);
}

void HSVHist::removeBack(HSVHist dst, HSVHist back)
{
	dst.Init();
	back.Init();
	dst.removeSeg(back);
}
