/**
* BackgroundSubtraction_factory.cpp
* Abstract class for developing a object factory. 
* Implementation of the commong methods for every background subtractor.
*
* Libraries: OpenNI, OpenCV.
*
* Author: Emilio J. Almazan <emilio.almazan@kingston.ac.uk>, 2012
*/
#include "BackgroundSubtraction_factory.h"

	
const void* BackgroundSubtraction_factory::getBackgroundModel()
{
	return backGroundModel;
}

void BackgroundSubtraction_factory::createBackImage(const XnPoint3D* points2D, IplImage* backImg, int numPoints)
{
	int cont;
	for (cont = 0; cont < numPoints; cont++)
	{
		XnPoint3D p = points2D[cont];
		int y = (int)p.Y;
		int x = (int)p.X;
		uchar* ptr = (uchar*)(backImg->imageData + (y*(backImg->widthStep)));
		ptr[x] = 255;
		cont++;
	}
}