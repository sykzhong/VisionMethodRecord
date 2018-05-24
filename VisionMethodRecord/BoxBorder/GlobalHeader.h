#ifndef _GLOBALHEADER_H_
#define _GLOBALHEADER_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>  
#include <stdio.h>
#include "easylogging++.h"

using namespace cv;
using namespace std;
using namespace el;

static const Scalar RED = Scalar(0, 0, 255);
static const Scalar PINK = Scalar(230, 130, 255);
static const Scalar BLUE = Scalar(255, 0, 0);
static const Scalar LIGHTBLUE = Scalar(255, 255, 160);
static const Scalar GREEN = Scalar(0, 255, 0);
static const Scalar YELLOW = Scalar(0, 255, 255);
static const Scalar ORCHID = Scalar(214, 112, 218);

static const Scalar BLACK = Scalar(0, 0, 0);
static const Scalar WHITE = Scalar(255, 255, 255);

static const double PI = 3.141592654;

#endif