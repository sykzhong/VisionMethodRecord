#include <iostream>
#include <opencv2\opencv.hpp>
using namespace std;

class Watershed_auto
{
public:
	Watershed_auto();
	~Watershed_auto();

	void getImage();
	void threshImage();

	void getForeBackImage();
	void getMarkers();
	void processWatershed();
	void showResult();

private:
	cv::Mat srcimage;
	cv::Mat threshed_image;
	cv::Mat fore_image;
	cv::Mat back_image;
	cv::Mat unsure_image;

	cv::Mat markers;

	double maxmark;
};

Watershed_auto::Watershed_auto():
	back_image(cv::Mat(cv::Size(), CV_32FC1)),
	fore_image(cv::Mat(cv::Size(), CV_32FC1))
{

}

Watershed_auto::~Watershed_auto()
{

}

void Watershed_auto::getImage()
{
	srcimage = cv::imread("Pictures//watershed_test2.jpg", CV_LOAD_IMAGE_COLOR);
	//imshow("srcimage", srcimage);
	//cv::waitKey(0);
}

void Watershed_auto::threshImage()
{
	cv::Mat srcimage_gray;
	cv::cvtColor(srcimage, srcimage_gray, CV_RGB2GRAY);
	threshed_image = cv::Mat(srcimage.size(), CV_8UC1);
	//cv::threshold(srcimage_gray, threshed_image, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
	cv::threshold(srcimage_gray, threshed_image, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	imshow("threshed_image", threshed_image);
	cv::waitKey(0);
}

void Watershed_auto::getForeBackImage()
{
	int morph_elem = 2;		//Element:\n 0: Rect - 1: Cross - 2: Ellipse
	int morph_size = 3;
	int morph_operation = 0;		//0: Opening - 1: Closing \n 2: Gradient - 3: Top Hat \n 4: Black Hat
	cv::Mat element = cv::getStructuringElement(morph_elem, cv::Size(2 * morph_size + 1, 2 * morph_size + 1), cv::Point(morph_size, morph_size));
	cv::Mat opening;
	cv::morphologyEx(threshed_image, opening, morph_operation, element, cv::Point(-1, -1), 2);
	imshow("opening", opening);
	cv::waitKey(0);

	cv::dilate(threshed_image, back_image, element, cv::Point(-1, -1), 3);
	imshow("back_image", back_image);
	cv::waitKey(0);

	cv::Mat distance;
	cv::distanceTransform(opening, distance, CV_DIST_L2, 5);
	//distance.convertTo(distance, CV_8UC1);
	//imshow("distance", distance);		//sykfix: 做归一化？
	//归一化

	//cv::waitKey(0);
	
	double maxdistance;
	cv::minMaxLoc(distance, NULL, &maxdistance);
	cout << maxdistance;
	cv::threshold(distance, fore_image, 0.03*maxdistance, 255, 0);
	imshow("fore_image", fore_image);
	cv::waitKey(0);

	fore_image.convertTo(fore_image, CV_8UC1);
	back_image.convertTo(back_image, CV_8UC1);
	cv::subtract(back_image, fore_image, unsure_image);
	imshow("unsure_image", unsure_image);
	cv::waitKey(0);
}

void Watershed_auto::getMarkers()
{
	cv::connectedComponents(fore_image, markers);
	markers = markers + 1;
	markers.convertTo(markers, CV_32S);
	unsure_image.convertTo(unsure_image, CV_32S);
	cv::minMaxLoc(markers, NULL, &maxmark);

	int nChannels = markers.channels();
	int nRows = markers.rows;
	int nCols = markers.cols;

	int Channels = unsure_image.channels();
	int Rows = unsure_image.rows;
	int Cols = unsure_image.cols;
	int *p;			//指代markers的行指针
	const int *q;		//指代unsure_image的行指针
					
	int index = 0;		//用于记录三通道的末下标

	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			p = markers.ptr<int>(i);
			q = unsure_image.ptr<int>(i);
			//LOG(TRACE) << (int)(q[j*nChannels]);
			if (q[j*nChannels] != 0)
				p[j*nChannels] = 0;
		}

	}

	cout << maxmark;
	//markers(unsure_image) = 0;
}

void Watershed_auto::processWatershed()
{
	watershed(srcimage, markers);
}

void Watershed_auto::showResult()
{
	vector<cv::Vec3b> colorTab;  // 随机生成几种颜色
	cv::Mat markers_copy;
	markers.copyTo(markers_copy);
	for (int i = 0; i < maxmark; i++)
	{
		int b = cv::theRNG().uniform(0, 255);
		int g = cv::theRNG().uniform(0, 255);
		int r = cv::theRNG().uniform(0, 255);

		colorTab.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
	}

	cv::Mat resImage = cv::Mat(srcimage.size(), CV_8UC3);  // 声明一个最后要显示的图像
	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{	// 根据经过watershed处理过的maskWaterShed来绘制每个区域的颜色
			int index = markers.at<int>(i, j);  // 这里的maskWaterShed是经过watershed处理的
			if (index == -1)  // 区域间的值被置为-1（边界）
				resImage.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
			else if (index <= 0 || index > maxmark)  // 没有标记清楚的区域被置为0
				resImage.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
			else  // 其他每个区域的值保持不变：1，2，...，contours.size()
				resImage.at<cv::Vec3b>(i, j) = colorTab[index - 1];  // 然后把这些区域绘制成不同颜色
		}
	}
	//addWeighted(resImage, 0.3, srcimage, 0.7, 0, resImage);
	imshow("result", resImage);
	imshow("markers", markers);
	cv::waitKey(0);
}

int main()
{
	Watershed_auto watershed_test;
	watershed_test.getImage();
	watershed_test.threshImage();
	watershed_test.getForeBackImage();
	watershed_test.getMarkers();
	//watershed_test.processWatershed();
	watershed_test.showResult();
	system("pause");
}