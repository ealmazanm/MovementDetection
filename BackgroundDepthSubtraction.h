/**
* Header file of BackgroundDepthSustraction class
*
* Libraries: OpenNI, OpenCV.
*
* Author: Emilio J. Almazan <emilio.almazan@kingston.ac.uk>, 2012
*/
#pragma once
#include "BackgroundSubtraction_factory.h"
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



class BackgroundDepthSubtraction : public BackgroundSubtraction_factory
{
public:
	BackgroundDepthSubtraction(const XnDepthPixel*);
	~BackgroundDepthSubtraction(void);

	//Performa a depth background sustraction
	virtual int subtraction(XnPoint3D* points2D, const void* currentMap);

};

