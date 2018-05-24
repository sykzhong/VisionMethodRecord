#include "HSVHist.h"
#include <fstream>
#include <stdio.h>
#include <time.h>

INITIALIZE_EASYLOGGINGPP

void getTargetAreaImage()
{
	string str_front = ".\\HSVHist\\0523_front_3.jpg", str_back = ".\\HSVHist\\0523_back.jpg";
	HSVHist img_front, img_back;

	img_front.getImage(str_front);
	img_back.getImage(str_back);


	//img_front.preprocess();
	//img_back.preprocess();

	HSVHist::removeBack(img_front, img_back);
	img_front.drawHist();
	img_back.drawHist();
	img_front.showImage();			//result img name: "houghtest.jpg"
	img_front.getTargetContour();
	waitKey(0);
}





int main()
{
	getTargetAreaImage();
}