#include "BackgroundColorSubtraction.h"


BackgroundColorSubtraction::BackgroundColorSubtraction(const IplImage* img)
{
	//create a gray scale image for the backGroundModel
	backGroundModel = cvCloneImage(img);	
}


BackgroundColorSubtraction::~BackgroundColorSubtraction(void)
{
	cvReleaseImage((IplImage**)&backGroundModel);
}


//Public methods implementation
int BackgroundColorSubtraction::subtraction(vector<XnPoint3D>* points2D, const void* currentGray)
{
	int cont = 0;
	//perform the subtraction (|current(x,y)-back(x,y)|>BGS_THRESHOLD. Add points to the list
	for (int y = 0; y < XN_VGA_Y_RES; y++)
	{
		uchar* ptr_Back = (uchar*)((IplImage*)backGroundModel)->imageData + (y*((IplImage*)backGroundModel)->widthStep);
		uchar* ptr_Curr = (uchar*)((IplImage*)currentGray)->imageData + (y*((IplImage*)currentGray)->widthStep);
		for (int x = 0; x < XN_VGA_X_RES; x++)
		{
			float current_color = ptr_Curr[x];
			float back_color = ptr_Back[x];
			if (abs(current_color-back_color) > BGS_THRESHOLD)
			{
				XnPoint3D p;
				p.X = (XnFloat)x; p.Y = (XnFloat)y; //p.Z = (XnFloat)current_depth;
				(*points2D)[cont++] = p;
			}
			//update the background (alpha*Current(x,y) + (1-alpha)*back(x,y))
			ptr_Back[x] = ALPHA*current_color + (1-ALPHA)*back_color;			
		}
	}
	return 0;
}