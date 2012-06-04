/**
* Header file of BackgroundDepthSustraction class
*
* Libraries: OpenNI, OpenCV.
*
* Author: Emilio J. Almazan <emilio.almazan@kingston.ac.uk>, 2012
*/
#pragma once
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <list>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <ctype.h>
#include "XnCppWrapper.h"
#include "Utils.h"
#include "KinectSensor.h"

using namespace std;
using namespace cv;
using namespace xn;


//const int MAX_DEPTH = 10000;
const double ALPHA_FOREGROUND = 0.02;
const double ALPHA_BACKGROUND = 0.005;
//Threshold for the background depth subtraction (mm)
const int BGS_THRESHOLD = 400;

const int MAX_FORGROUND_POINTS = 80*XN_VGA_Y_RES*XN_VGA_X_RES/100;


class BackgroundDepthSubtraction
{
public:
	BackgroundDepthSubtraction();
	BackgroundDepthSubtraction(const XnDepthPixel*);
	~BackgroundDepthSubtraction(void);

	inline const XnDepthPixel* getBackgroundModel()
	{
		return backGroundModel;
	}

	void createBackImage(const XnPoint3D* points2D, Mat& backImg, const int numPoints);
	void initBackgroundModel(const XnDepthPixel*);

	//Performa a depth background sustraction
	int subtraction(XnPoint3D* points2D, const XnDepthPixel* currentDepth);

private:
	XnDepthPixel* backGroundModel;
	bool init;
};

