/**
* BackgroundDepthSustraction.cpp: Implements a background sustraction
* using depth.
*
* Libraries: OpenNI, OpenCV.
*
* Author: Emilio J. Almazan <emilio.almazan@kingston.ac.uk>, 2012
*/
#include "BackgroundDepthSubtraction.h"


BackgroundDepthSubtraction::BackgroundDepthSubtraction(const XnDepthPixel* depthMap)
{
	backGroundModel = new XnDepthPixel[XN_VGA_Y_RES*XN_VGA_X_RES];
	Utils::copyDepthMap(depthMap, (XnDepthPixel*)backGroundModel);
}


BackgroundDepthSubtraction::~BackgroundDepthSubtraction(void)
{
	delete(backGroundModel);
}


//Public methods implementation
int BackgroundDepthSubtraction::subtraction(XnPoint3D* points2D, const void* currentDepth)
{
	const XnDepthPixel* currentMap = (const XnDepthPixel*)currentDepth;
	XnDepthPixel* backMat = (XnDepthPixel*)backGroundModel;
	int cont = 0;
	//perform the subtraction (|current(x,y)-back(x,y)|>BGS_THRESHOLD. Add points to the list
	for (int y = 0; y < XN_VGA_Y_RES; y++)
	{
		for (int x = 0; x < XN_VGA_X_RES; x++)
		{
			float curretnVal = currentMap[y*XN_VGA_X_RES+x];
			float backVal = backMat[y*XN_VGA_X_RES+x];
			if (curretnVal != 0  && backVal != 0) 
			{
				if (abs(curretnVal-backVal) > BGS_THRESHOLD)
				{
					if (cont >= MAX_FORGROUND_POINTS)
					{
						cout << "Error: Not enough memory allocated" << endl;
						break;
					}
					else
					{
						XnPoint3D* p = new XnPoint3D;
						p->X = (XnFloat)x; p->Y = (XnFloat)y; p->Z = (XnFloat)curretnVal;
						points2D[cont++] = *p; 
						//update the background (alpha*Current(x,y) + (1-alpha)*back(x,y))
						backMat[y*XN_VGA_X_RES+x] = ALPHA_FOREGROUND*curretnVal + (1-ALPHA_FOREGROUND)*backVal;
					}
				}
				else
					backMat[y*XN_VGA_X_RES+x] = ALPHA_BACKGROUND*curretnVal + (1-ALPHA_BACKGROUND)*backVal;
				
			}
			
		}
	}
	return cont;
}
