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
	contInit = 0;
	init = false;
	labelImg = cvCreateImage(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_LABEL, 1);
}

BackgroundDepthSubtraction::BackgroundDepthSubtraction(const XnDepthPixel* depthMap)
{
	backGroundModel = new XnDepthPixel[XN_VGA_Y_RES*XN_VGA_X_RES];
	Utils::copyDepthMap(depthMap, backGroundModel);
}


BackgroundDepthSubtraction::~BackgroundDepthSubtraction(void)
{
}


//Public methods implementation
void BackgroundDepthSubtraction::initBackgroundModel(const XnDepthPixel* depthMap)
{
	if (contInit == 0)
	{
		backGroundModel = new XnDepthPixel[XN_VGA_Y_RES*XN_VGA_X_RES];
		Utils::copyDepthMap(depthMap, backGroundModel);
	}
	else
	{
		for (int i = 0; i < XN_VGA_Y_RES; i++)
		{
			for (int j = 0; j < XN_VGA_X_RES; j++)
			{
				int modelVal = backGroundModel[i*XN_VGA_X_RES+j];
				int curreVal = depthMap[i*XN_VGA_X_RES+j];
				if (modelVal == 0 && curreVal != 0)
					backGroundModel[i*XN_VGA_X_RES+j] = curreVal;
			}
		}
	}

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

ofstream outDebug("d:\\Debug.txt", ios::out);
//CurrentDepth: CV_16U (ushort)
//Mask: mask of noise (noise = 1)
int BackgroundDepthSubtraction::subtraction(XnPoint3D* points2D, Mat* currentDepth, Mat* mask)
{
	int nPeop = 0;
	int cont = 0;
	//Create a model without moving object.
	//It also reduces the blinking noise
	if (contInit < NUM_INIT_FRAMES)
	{
		if (contInit == 0)
		{
			currentDepth->copyTo(backgroundModel_img);
			mask->copyTo(maskModel);
		}
		else
		{
			cv::add(*currentDepth, backgroundModel_img, backgroundModel_img, maskModel);
			bitwise_and(*mask, maskModel, maskModel);
		}
		contInit++;
	}
	else
	{
		Mat out;
		//Assigned bg Model values to current img null values. So the subtraction does only apply to null values
		cv::add(*currentDepth, backgroundModel_img, *currentDepth, *mask);
		absdiff(*currentDepth, backgroundModel_img, out); //out is type CV_16U
		Mat bw = out > BGS_THRESHOLD; //bw is type CV_16U
		//Morphological operation
		Mat m;
		erode(bw, bw, m); //bw it still has noise in form of small regions
		//Connected components
		IplImage img = bw;
		unsigned int result = cvLabel(&img, labelImg, blobs);
//		cvFilterByLabel(blobs,cvGreaterBlob(blobs));
		//Filtering the blobs
		cvFilterByArea(blobs,5000,500000);

		//label_mask is the output image with the foreground objects (no noise)
		//used for the updating step
		Mat label_mask = Mat::zeros(bw.size(), CV_8U); 
		for (cvb::CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
		{
			int maxX = it->second->maxx;
			int maxY = it->second->maxy;
			int minX = it->second->minx;
			int minY = it->second->miny;
			Rect roi(minX, minY, maxX-minX, maxY-minY);
			Mat roi_label = label_mask(roi);
			Mat roi_bw = bw(roi);

			//peopleOut[nPeop++] = roi;

			//Updates the 0 values of roi_label with the ushort_Max values of roi_bw (foreground)
			bitwise_or(roi_label, roi_bw, roi_label); //0 and ushort_Max values in roi_label

			//Obtain all the pixels (this is for the demo, it will have to return people instead of points)
			for (int i = minY; i < maxY; i++)
			{
				for (int j = minX; j < maxX; j++)
				{
					//ushort maskVal = bw.ptr<uchar>(i)[j];
					ushort maskVal = label_mask.ptr<uchar>(i)[j];
					if (maskVal == 255)
					{
						ushort curretnVal = currentDepth->ptr<ushort>(i)[j];
						XnPoint3D p;
						p.X = (XnFloat)j; p.Y = (XnFloat)i; p.Z = (XnFloat)curretnVal;
						points2D[cont++] = p; 
					}
				}
			}

		}

		//updates bg and fg pixels of the model
		Mat tmp1, tmp2;
		tmp1 = ALPHA_FOREGROUND* (*currentDepth);
		tmp2 = (1-ALPHA_FOREGROUND)* backgroundModel_img;
		cv::add(tmp1, tmp2, backgroundModel_img, label_mask); //add only in the elements of the mask not 0

		tmp1 = ALPHA_BACKGROUND* (*currentDepth);
		tmp2 = (1-ALPHA_BACKGROUND)* backgroundModel_img;
		Mat label_mask_I;
		cv::bitwise_not(label_mask, label_mask_I);

		cv::add(tmp1, tmp2, backgroundModel_img, label_mask_I);	
		label_mask.copyTo(*mask);


	}
	//return nPoints;
	return cont;
}


/*
Detection of foreground objects.
Parametres:
IN--currentDepth: Ushort matrix of depths in mm.
IN--mask: Noise mask (noise != 0)
OUT--peopleOut: Object containing all the people in the image

Return number of people detected
*/
void BackgroundDepthSubtraction::subtraction(Mat* currentDepth, Mat* mask, ForgroundObjs* peopleOut)
{
	int nPeop = 0;
	Rect people[10];
	int cont = 0;
	//Create a model without moving object.
	//It also reduces the blinking noise
	if (contInit < NUM_INIT_FRAMES)
	{
		if (contInit == 0)
		{
			currentDepth->copyTo(backgroundModel_img);
			mask->copyTo(maskModel);
		}
		else
		{
			cv::add(*currentDepth, backgroundModel_img, backgroundModel_img, maskModel);
			bitwise_and(*mask, maskModel, maskModel);
		}
		contInit++;
	}
	else
	{
		Mat out;
		//Assigned bg Model values to current img null values. So the subtraction does only apply to null values
		cv::add(*currentDepth, backgroundModel_img, *currentDepth, *mask);
		absdiff(*currentDepth, backgroundModel_img, out); //out is type CV_16U
		Mat bw = out > BGS_THRESHOLD; //bw is type CV_16U
		//Morphological operation
		Mat m;
		erode(bw, bw, m); //bw it still has noise in form of small regions
		//Connected components
		IplImage img = bw;
		unsigned int result = cvLabel(&img, labelImg, blobs);
//		cvFilterByLabel(blobs,cvGreaterBlob(blobs));
		//Filtering the blobs
		cvFilterByArea(blobs,5000,500000);

		//label_mask is the output image with the foreground objects (no noise)
		//used for the updating step
		Mat label_mask = Mat::zeros(bw.size(), CV_8U);
		Mat zero_mask = Mat::zeros(bw.size(), CV_16U); 
		//image that stores only the depth of the foreground pixels. the rest values are 0.
		Mat fDepth = Mat::zeros(bw.size(), CV_16U); 
		for (cvb::CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
		{
			int maxX = it->second->maxx;
			int maxY = it->second->maxy;
			int minX = it->second->minx;
			int minY = it->second->miny;
			Rect roi(minX, minY, maxX-minX, maxY-minY);
			Mat roi_label = label_mask(roi);
			Mat roi_bw = bw(roi);

			people[nPeop++] = roi;

			//Updates the 0 values of roi_label with the 255 values of roi_bw (foreground)
			bitwise_or(roi_label, roi_bw, roi_label); //0 and 255 values in roi_label
		}
		if (nPeop > 0)
		{
			peopleOut->setNumObj(nPeop);
			peopleOut->setBBoxes(people);
			cv::add(zero_mask, *currentDepth, fDepth, label_mask);
			peopleOut->setForImg(fDepth);
			peopleOut->recoverFPoints();
		}

		//updates bg and fg pixels of the model
		Mat tmp1, tmp2;
		tmp1 = ALPHA_FOREGROUND* (*currentDepth);
		tmp2 = (1-ALPHA_FOREGROUND)* backgroundModel_img;
		cv::add(tmp1, tmp2, backgroundModel_img, label_mask); //add only in the elements of the mask not 0

		tmp1 = ALPHA_BACKGROUND* (*currentDepth);
		tmp2 = (1-ALPHA_BACKGROUND)* backgroundModel_img;
		Mat label_mask_I;
		cv::bitwise_not(label_mask, label_mask_I);

		cv::add(tmp1, tmp2, backgroundModel_img, label_mask_I);	
		label_mask.copyTo(*mask);


	}
}


int BackgroundDepthSubtraction::subtraction(XnPoint3D* points2D, const XnDepthPixel* currentDepth)
{
	if (contInit < NUM_INIT_FRAMES)
	{
		initBackgroundModel(currentDepth);
		contInit++;
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
			if (curretnVal != 0 )// && backVal != 0) 
			{
				if ((abs(curretnVal-backVal) > BGS_THRESHOLD))// && (curretnVal < backVal)) // second condition: not include the shadow points
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
