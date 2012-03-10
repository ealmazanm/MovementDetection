/**
* BackgroundSubtraction_factory.h
* Abstract class for developing a object factory. 
* The factory creates different ways of implementing the background subtraction.
*
* Libraries: OpenNI, OpenCV.
*
* Author: Emilio J. Almazan <emilio.almazan@kingston.ac.uk>, 2012
*/
#pragma once
#include "XnCppWrapper.h"
#include "opencv/cv.h"


const int MAX_DEPTH = 10000;
const double ALPHA = 0.05;
//Threshold for the background depth subtraction (mm)
const int BGS_THRESHOLD = 100;

class BackgroundSubtraction_factory
{
public:

	//Abstract method to be implemented. Perfoms a background subtraction for movement detection
	virtual void subtraction(vector<XnPoint3D>* points2D, const void* currentMap) = 0;

	//Return the background model
	const void* getBackgroundModel();

	//Update the background image with the points that have moved in t-1.
	void createBackImage(const vector<XnPoint3D>* points2D, IplImage* backImg);

protected:
		// background model
		void* backGroundModel;
};