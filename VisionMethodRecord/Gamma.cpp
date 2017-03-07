#include "stdafx.h"
#include "ContourExtraction.h"

Mat& GammaCorrection(Mat& src, float fGamma)
{
	CV_Assert(src.data);  //若括号中的表达式为false，则返回一个错误的信息。  

						  // accept only char type matrices    
	CV_Assert(src.depth() != sizeof(uchar));
	// build look up table    
	unsigned char lut[256];
	for (int i = 0; i < 256; i++)
	{
		lut[i] = pow((float)(i / 255.0), fGamma) * 255.0;
	}
	//先归一化，i/255,然后进行预补偿(i/255)^fGamma,最后进行反归一化(i/255)^fGamma*255  

	const int channels = src.channels();
	switch (channels)
	{
	case 1:
		{
			//运用迭代器访问矩阵元素  
			MatIterator_<uchar> it, end;
			for (it = src.begin<uchar>(), end = src.end<uchar>(); it != end; it++)
				*it = lut[(*it)];
			break;
		}
	case 3:
		{

			MatIterator_<Vec3b> it, end;
			for (it = src.begin<Vec3b>(), end = src.end<Vec3b>(); it != end; it++)
			{
				(*it)[0] = lut[((*it)[0])];
				(*it)[1] = lut[((*it)[1])];
				(*it)[2] = lut[((*it)[2])];
			}
			break;
		}
	}
	return src;
}