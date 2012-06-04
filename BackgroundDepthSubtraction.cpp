/**
* BackgroundDepthSustraction.cpp: Implements a background sustraction
* using depth.
*
* Libraries: OpenNI, OpenCV.
*
* Author: Emilio J. Almazan <emilio.almazan@kingston.ac.uk>, 2012
*/
#include "BackgroundDepthSubtraction.h"


BackgroundDepthSubtraction::BackgroundDepthSubtraction()
{
	init = false;
}

BackgroundDepthSubtraction::BackgroundDepthSubtraction(const XnDepthPixel* depthMap)
{
	backGroundModel = new XnDepthPixel[XN_VGA_Y_RES*XN_VGA_X_RES];
	Utils::copyDepthMap(depthMap, backGroundModel);
}


BackgroundDepthSubtraction::~BackgroundDepthSubtraction(void)
{
	delete(backGroundModel);
}


//Public methods implementation
void BackgroundDepthSubtraction::initBackgroundModel(const XnDepthPixel* depthMap)
{
	backGroundModel = new XnDepthPixel[XN_VGA_Y_RES*XN_VGA_X_RES];
	Utils::copyDepthMap(depthMap, backGroundModel);

}

void BackgroundDepthSubtraction::createBackImage(const XnPoint3D* points2D, Mat& backImg, const int numPoints)
{
	int cont;
	for (cont = 0; cont < numPoints; cont++)
	{
		XnPoint3D p = points2D[cont];
		int y = (int)p.Y;
		int x = (int)p.X;
		uchar* ptr = backImg.ptr<uchar>(y);
		ptr[x] = 255;
	}
}

int BackgroundDepthSubtraction::subtraction(XnPoint3D* points2D, const XnDepthPixel* currentDepth)
{
	if (!init)
	{
		initBackgroundModel(currentDepth);
		init = true;
		return 0;
	}

	int cont = 0;
	//perform the subtraction (|current(x,y)-back(x,y)|>BGS_THRESHOLD. Add points to the list
	for (int y = 0; y < XN_VGA_Y_RES; y++)
	{
		for (int x = 0; x < XN_VGA_X_RES; x++)
		{
			float curretnVal = currentDepth[y*XN_VGA_X_RES+x];
			float backVal = backGroundModel[y*XN_VGA_X_RES+x];
			if (curretnVal != 0  && backVal != 0) 
			{
				if ((abs(curretnVal-backVal) > BGS_THRESHOLD) && (curretnVal < backVal)) // second condition: not include the shadow points
				{
					if (cont >= MAX_FORGROUND_POINTS)
					{
						cout << "Error: Not enough memory allocated" << endl;
						break;
					}
					else
					{
						XnPoint3D p;
						p.X = (XnFloat)x; p.Y = (XnFloat)y; p.Z = (XnFloat)curretnVal;
						points2D[cont++] = p; 
						//update the background (alpha*Current(x,y) + (1-alpha)*back(x,y))
						backGroundModel[y*XN_VGA_X_RES+x] = ALPHA_FOREGROUND*curretnVal + (1-ALPHA_FOREGROUND)*backVal;
					}
				}
				else
					backGroundModel[y*XN_VGA_X_RES+x] = ALPHA_BACKGROUND*curretnVal + (1-ALPHA_BACKGROUND)*backVal;
				
			}
			
		}
	}
	return cont;
}
