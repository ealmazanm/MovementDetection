/**
* BackgroundDepthSustraction.cpp: Implements a background sustraction
* using depth.
*
* Libraries: OpenNI, OpenCV.
*
* Author: Emilio J. Almazan <emilio.almazan@kingston.ac.uk>, 2012
*/
#include "BackgroundDepthSubtraction.h"


BackgroundDepthSubtraction::BackgroundDepthSubtraction(const XnDepthPixel* backDepths)
{
	backGroundDepth = new XnDepthPixel[XN_VGA_X_RES*XN_VGA_Y_RES];
	Utils::copyDepthMap(backDepths, backGroundDepth);
}


BackgroundDepthSubtraction::~BackgroundDepthSubtraction(void)
{
	delete(backGroundDepth);
}


//Public methods implementation
void BackgroundDepthSubtraction::subtraction(vector<XnPoint3D>* points2D, const XnDepthPixel* currentDepthMap)
{
	//perform the subtraction (|current(x,y)-back(x,y)|>BGS_THRESHOLD. Add points to the list
	for (int y = 0; y < XN_VGA_Y_RES; y++)
	{
		for (int x = 0; x < XN_VGA_X_RES; x++)
		{
			float current_depth = currentDepthMap[y*XN_VGA_X_RES+x];
			float back_depth = backGroundDepth[y*XN_VGA_X_RES+x];
			if (back_depth != 0 && current_depth != 0) 
			{
				if (abs(current_depth-back_depth) > BGS_THRESHOLD)
				{
					XnPoint3D p;
					p.X = x; p.Y = y; p.Z = current_depth;
					points2D->push_back(p);
				}
				//update the background (alpha*Current(x,y) + (1-alpha)*back(x,y))
				backGroundDepth[y*XN_VGA_X_RES+x] = ALPHA*current_depth + (1-ALPHA)*back_depth;
			}
		}
	}
}

const XnDepthPixel* BackgroundDepthSubtraction::getBackgroundDepth()
{
	return backGroundDepth;
}

void BackgroundDepthSubtraction::createBackImage(const vector<XnPoint3D>* points2D, IplImage* backImg)
{
	vector<XnPoint3D>::const_iterator iter;
	for (iter = points2D->begin(); iter != points2D->end(); iter++)
	{
		XnPoint3D p = *iter;
		int y = p.Y;
		int x = p.X;
		uchar* ptr = (uchar*)(backImg->imageData + (y*(backImg->widthStep)));
		ptr[x] = 255;
	}
}