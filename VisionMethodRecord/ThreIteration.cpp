#include "stdafx.h"
#include "ContourExtraction.h"
const int maxThValue = 255;
void ThreIteration(Mat const &image)
{
	if (!image.data)
		return;

	Mat imageTemp;
	imageTemp = image.clone();
	//cvtColor(imageTemp, imageTemp, CV_RGB2GRAY);
	imageTemp.reshape(1, 0);						//����ͼ��ת��Ϊ��ͨ��

	double Zavr, Zmin, Zmax;
	//int *minIdx, *maxIdx;
	Point Pmin, Pmax;
	
	minMaxLoc(imageTemp, &Zmin, &Zmax, &Pmin, &Pmax);	
	Zavr = (Zmin + Zmax) / 2;					//���ͼ��ƽ���Ҷ�

	Mat imageFore, imageBack;						//����ͼ��ǰ���뱳����ǰ���Ҷȵ�
	double thTemp;
	int itercount = 1;
	while (1)
	{
		threshold(imageTemp, imageFore, Zavr, maxThValue, THRESH_TRUNC);
		imageBack = imageTemp - imageFore;
		Zmin = mean(imageFore, imageFore)[0];
		Zmax = mean(imageBack, imageBack)[0];
		thTemp = (Zmin + Zmax) / 2;

		if ((abs(thTemp - Zavr) < 0.5) || itercount >= 100)
			break;
		else
		{
			Zavr = thTemp;
			itercount++;
		}
	}
	threshold(image, image, Zavr, maxThValue, THRESH_BINARY);

	//threshold(image, image, Zavr, maxThValue, THRESH_BINARY);
	return;
}