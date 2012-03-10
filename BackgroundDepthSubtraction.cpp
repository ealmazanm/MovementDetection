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
	backGroundModel = new XnDepthPixel[XN_VGA_X_RES*XN_VGA_Y_RES];
	Utils::copyDepthMap(backDepths, (XnDepthPixel*)backGroundModel);
}


BackgroundDepthSubtraction::~BackgroundDepthSubtraction(void)
{
	delete(backGroundModel);
}


//Public methods implementation
void BackgroundDepthSubtraction::subtraction(vector<XnPoint3D>* points2D, const void* currentMap)
{
	//perform the subtraction (|current(x,y)-back(x,y)|>BGS_THRESHOLD. Add points to the list
	for (int y = 0; y < XN_VGA_Y_RES; y++)
	{
		for (int x = 0; x < XN_VGA_X_RES; x++)
		{
			float current_depth = ((const XnDepthPixel*)currentMap)[y*XN_VGA_X_RES+x];
			float back_depth = ((XnDepthPixel*)backGroundModel)[y*XN_VGA_X_RES+x];
			if (back_depth != 0 && current_depth != 0) 
			{
				if (abs(current_depth-back_depth) > BGS_THRESHOLD)
				{
					XnPoint3D p;
					p.X = (XnFloat)x; p.Y = (XnFloat)y; p.Z = (XnFloat)current_depth;
					points2D->push_back(p);
				}
				//update the background (alpha*Current(x,y) + (1-alpha)*back(x,y))
				((XnDepthPixel*)backGroundModel)[y*XN_VGA_X_RES+x] = ALPHA*current_depth + (1-ALPHA)*back_depth;
			}
		}
	}
}