
#include <iostream>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

Mat srcImage, srcImage_, maskImage;
Mat maskWaterShed;  // watershed()�����Ĳ���
Point clickPoint;	// ������ȥ��λ��

void on_Mouse(int event, int x, int y, int flags, void*);
void helpText();
int main(int argc, char** argv)
{
	/* ������ʾ */
	helpText();

	srcImage = imread("Pictures//watershedtest.jpg");
	srcImage_ = srcImage.clone();  // ������srcImage�ᱻ�ı䣬��������������
	maskImage = Mat(srcImage.size(), CV_8UC1);  // ��ģ������������ǣ�Ȼ�󴫸�findContours
	maskImage = Scalar::all(0);

	int areaCount = 1;  // �������ڰ���0��ʱ����ÿ������
	namedWindow("��ͼ���������", CV_WINDOW_NORMAL);
	imshow("��ͼ���������", srcImage);

	setMouseCallback("��ͼ���������", on_Mouse, 0);

	while (true)
	{
		int c = waitKey(0);

		if ((char)c == 27)	// ����ESC�����˳�
			break;

		if ((char)c == '2')  // ����2���ָ�ԭͼ
		{
			maskImage = Scalar::all(0);
			srcImage = srcImage_.clone();

			namedWindow("��ͼ���������", CV_WINDOW_NORMAL);
			imshow("��ͼ���������", srcImage);
		}

		if ((char)c == '1')  // ����1������ͼƬ
		{
			vector<vector<Point>> contours;
			vector<Vec4i> hierarchy;

			findContours(maskImage, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

			if (contours.size() == 0)  // ���û������ǣ���û�����������˳���if���
				break;
			cout << contours.size() << "������" << endl;

			maskWaterShed = Mat(maskImage.size(), CV_32S);
			maskWaterShed = Scalar::all(0);

			/* ��maskWaterShed�ϻ������� */
			for (int index = 0; index < contours.size(); index++)
				drawContours(maskWaterShed, contours, index, Scalar::all(index + 1), -1, 8, hierarchy, INT_MAX);

			/* ���imshow���maskWaterShed�����ǻᷢ������һƬ�ڣ�ԭ��������������ֻ��������1,2,3������ֵ��ͨ������80�еĴ������ǲ�������Ŀ������ */
			watershed(srcImage_, maskWaterShed);  // ע��һ

			vector<Vec3b> colorTab;  // ������ɼ�����ɫ
			for (int i = 0; i < contours.size(); i++)
			{
				int b = theRNG().uniform(0, 255);
				int g = theRNG().uniform(0, 255);
				int r = theRNG().uniform(0, 255);

				colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
			}

			Mat resImage = Mat(srcImage.size(), CV_8UC3);  // ����һ�����Ҫ��ʾ��ͼ��
			for (int i = 0; i < maskImage.rows; i++)
			{
				for (int j = 0; j < maskImage.cols; j++)
				{	// ���ݾ���watershed�������maskWaterShed������ÿ���������ɫ
					int index = maskWaterShed.at<int>(i, j);  // �����maskWaterShed�Ǿ���watershed�����
					if (index == -1)  // ������ֵ����Ϊ-1���߽磩
						resImage.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
					else if (index <= 0 || index > contours.size())  // û�б�������������Ϊ0
						resImage.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
					else  // ����ÿ�������ֵ���ֲ��䣺1��2��...��contours.size()
						resImage.at<Vec3b>(i, j) = colorTab[index - 1];  // Ȼ�����Щ������Ƴɲ�ͬ��ɫ
				}
			}
			namedWindow("resImage", CV_WINDOW_NORMAL);
			imshow("resImage", resImage);
			namedWindow("��ˮ����", CV_WINDOW_NORMAL);
			addWeighted(resImage, 0.3, srcImage_, 0.7, 0, resImage);
			imshow("��ˮ����", resImage);
		}

		if ((char)c == '0')  // ��ε㰴��0��������ʾÿ�����ָ��������Ҫ�Ȱ���1������ͼ��
		{
			Mat resImage = Mat(srcImage.size(), CV_8UC3);  // ����һ�����Ҫ��ʾ��ͼ��
			for (int i = 0; i < maskImage.rows; i++)
			{
				for (int j = 0; j < maskImage.cols; j++)
				{
					int index = maskWaterShed.at<int>(i, j);
					if (index == areaCount)
						resImage.at<Vec3b>(i, j) = srcImage_.at<Vec3b>(i, j);
					else
						resImage.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
				}
			}
			imshow("��ˮ����", resImage);
			areaCount++;
			if (areaCount == 4)
				areaCount = 1;
		}
	}

	return 0;
}

void on_Mouse(int event, int x, int y, int flags, void*)
{
	// �����겻�ڴ������򷵻�
	if (x < 0 || x >= srcImage.cols || y < 0 || y >= srcImage.rows)
		return;

	// ��������������£���ȡ��굱ǰλ�ã������������²����ƶ�ʱ�����ư��ߣ�
	if (event == EVENT_LBUTTONDOWN)
	{
		clickPoint = Point(x, y);
	}
	else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))
	{
		Point point(x, y);
		line(maskImage, clickPoint, point, Scalar::all(255), 5, 8, 0);
		line(srcImage, clickPoint, point, Scalar::all(255), 5, 8, 0);
		clickPoint = point;
		imshow("��ͼ���������", srcImage);
	}
}

void helpText()
{
	cout << "���������ͼƬ�����б�ǳ����µ�����" << endl;
	cout << "������ͼƬ�ָ�ΪN�����򣬾�Ҫ��N�����" << endl;
	cout << "���̰�����1��	- ���еķ�ˮ��ָ��㷨" << endl;
	cout << "���̰�����2��	- �ָ�ԭʼͼƬ" << endl;
	cout << "���̰�����0��	- ���ηָ�ÿ�����򣨱����Ȱ���1����" << endl;
	cout << "���̰�����ESC��	- �˳�����" << endl << endl;
}

/* ע��һ��watershed(srcImage_, maskWaterShed);
* ע��������������
* srcImage_��û���κ��޸ĵ�ԭͼ��CV_8UC3����
* maskWaterShed����ΪCV_32S���ͣ�32λ��ͨ��������ȫ��Ԫ��Ϊ0
*		Ȼ����ΪdrawContours�ĵ�һ���������룬�������������
*		�����Ϊwatershed�Ĳ���
* ���⣬watershed�ĵڶ�������maskWaterShed��InputOutputArray����
*		����Ϊ���룬Ҳ��Ϊ������溯�����õĽ��
*/