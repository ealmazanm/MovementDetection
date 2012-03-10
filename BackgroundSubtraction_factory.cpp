#include "BackgroundSubtraction_factory.h"

	
const void* BackgroundSubtraction_factory::getBackgroundModel()
{
	return backGroundModel;
}

void BackgroundSubtraction_factory::createBackImage(const vector<XnPoint3D>* points2D, IplImage* backImg)
{
	vector<XnPoint3D>::const_iterator iter;
	for (iter = points2D->begin(); iter != points2D->end(); iter++)
	{
		XnPoint3D p = *iter;
		int y = (int)p.Y;
		int x = (int)p.X;
		uchar* ptr = (uchar*)(backImg->imageData + (y*(backImg->widthStep)));
		ptr[x] = 255;
	}
}