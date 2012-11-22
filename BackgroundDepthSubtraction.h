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
#include <cvblob.h>
#include <ForgroundObjs.h>

using namespace std;
using namespace cv;
using namespace xn;


//const int MAX_DEPTH = 10000;
const double ALPHA_FOREGROUND = 0.000005;
const double ALPHA_BACKGROUND = 0.0000005;
//Threshold for the background depth subtraction (mm)
const int BGS_THRESHOLD = 200;

const int NUM_INIT_FRAMES = 10;

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
	int subtraction(XnPoint3D* points2D, Mat* currentDepth, Mat* mask, Rect* peopleOut);
	void subtraction(Mat* currentDepth, Mat* mask, ForgroundObjs* peopleOut);

private:
	XnDepthPixel* backGroundModel;
	Mat backgroundModel_img;
	Mat maskModel; //mask the noise (noise == 0)
	int contInit;
	bool init;

	IplImage* labelImg;
	cvb::CvBlobs blobs;
};

