/**
* Header file of BackgroundDepthSustraction class
*
* Libraries: OpenNI, OpenCV.
*
* Author: Emilio J. Almazan <emilio.almazan@kingston.ac.uk>, 2012
*/
#pragma once
#include "XnCppWrapper.h"
#include "CameraProperties.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "stdio.h"
#include "Utils.h"
#include "filePaths.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <new>
#include <list>

using namespace std;
using namespace xn;
using std::string;

const int MAX_DEPTH = 10000;
const float ALPHA = 0.0005;
//Threshold for the background depth subtraction (mm)
const int BGS_THRESHOLD = 100;



class BackgroundDepthSubtraction
{
public:
	BackgroundDepthSubtraction(const XnDepthPixel*);
	~BackgroundDepthSubtraction(void);

	//Performa a sustraction between currentDepth and backGroundDepth. The result is stored in the list points2D
	void subtraction(vector<XnPoint3D>* points2D, const XnDepthPixel* currentDepth);

	//Return the background model
	const XnDepthPixel* getBackgroundDepth();

	//Update the background image with the points that have moved in t-1.
	static void createBackImage(const vector<XnPoint3D>* points2D, IplImage* backImg);

	//Depth background model
	XnDepthPixel* backGroundDepth;

};

