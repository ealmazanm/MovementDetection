#pragma once
#include "backgroundsubtraction_factory.h"
#include <Utils.h>


class BackgroundColorSubtraction :
	public BackgroundSubtraction_factory
{
public:
	BackgroundColorSubtraction(const IplImage*);
	~BackgroundColorSubtraction(void);

	//Performa a color background sustraction
	virtual void subtraction(vector<XnPoint3D>* points2D, const void* currentMap);
};

