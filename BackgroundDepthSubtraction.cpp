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
//	unit_background = Mat::ones(Size(XN_VGA_X_RES, XN_VGA_Y_RES), CV_8UC1); 
//	backgroundModel_time = Mat::zeros(Size(XN_VGA_X_RES, XN_VGA_Y_RES), CV_8UC1); 
}

BackgroundDepthSubtraction::BackgroundDepthSubtraction(const XnDepthPixel* depthMap)
{
	backGroundModel = new XnDepthPixel[XN_VGA_Y_RES*XN_VGA_X_RES];
	Utils::copyDepthMap(depthMap, backGroundModel);
//	unit_background = Mat::ones(Size(XN_VGA_X_RES, XN_VGA_Y_RES), CV_8UC1); 
//	backgroundModel_time = Mat::zeros(Size(XN_VGA_X_RES, XN_VGA_Y_RES), CV_8UC1); 
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
ofstream outDebug2("d:\\Debug2.txt", ios::out);

void printValues(Mat& m)
{
	for (int i = 0; i < m.rows; i++)
	{
		ushort* ptr = m.ptr<ushort>(i);
		for (int j = 0; j < m.cols; j++)
		{
			outDebug2 << (int)ptr[j] << " ";
		}
		cout << endl;
	}
}


//CurrentDepth: CV_16U (ushort)
//Mask: mask of noise (noise = 1)
//int BackgroundDepthSubtraction::subtraction(XnPoint3D* points2D, Mat* currentDepth, Mat* mask)
//{
//	int nPeop = 0;
//	int cont = 0;
//	//Create a model without moving object.
//	//It also reduces the blinking noise
//	if (contInit < NUM_INIT_FRAMES)
//	{
//		if (contInit == 0)
//		{
//			currentDepth->copyTo(backgroundModel_img);
//			mask->copyTo(maskModel);
//
//			unit_background = Mat::ones(mask->size(), CV_8UC1); 
//			backgroundModel_time = Mat::zeros(mask->size(), CV_8UC1); 
//
//		}
//		else
//		{
//			cv::add(*currentDepth, backgroundModel_img, backgroundModel_img, maskModel);
//			bitwise_and(*mask, maskModel, maskModel);
//		}
//		contInit++;
//	}
//	else
//	{
//		Mat out;
//		//Assigned bg Model values to current img null values. So the subtraction does only apply to null values
//		cv::add(*currentDepth, backgroundModel_img, *currentDepth, *mask);
//		absdiff(*currentDepth, backgroundModel_img, out); //out is type CV_16U
//		Mat bw = out > BGS_THRESHOLD; //bw is type CV_16U
//		
//		//Morphological operation
//		Mat m;
//		erode(bw, bw, m); //bw it still has noise in form of small regions
//		
//		//Check static foreground pixels
//		cv::add(unit_background, backgroundModel_time, backgroundModel_time, bw); //add unity to the foreground pixels
//		Mat bwNot;
//		bitwise_xor(bw, unit_background, bwNot);
//		threshold(bwNot, bwNot, 1, 1, CV_THRESH_BINARY_INV);
//		//printValues(bwNot);
//		cv::subtract(backgroundModel_time, backgroundModel_time, backgroundModel_time, bwNot);// set to 0 the background
//		Mat bg_missed = backgroundModel_time > 100; //bw is type CV_16U
//
//
//		
//		//Connected components
//		IplImage img = bw;
//		unsigned int result = cvLabel(&img, labelImg, blobs);
////		cvFilterByLabel(blobs,cvGreaterBlob(blobs));
//		//Filtering the blobs
//		//cvFilterByArea(blobs,4000,500000);
//		cvFilterByArea(blobs,50,500000);
//
//		//label_mask is the output image with the foreground objects (no noise)
//		//used for the updating step
//		Mat label_mask = Mat::zeros(bw.size(), CV_8U); 
//		for (cvb::CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
//		{
//			
//			int maxX = it->second->maxx;
//			int maxY = it->second->maxy;
//			int minX = it->second->minx;
//			int minY = it->second->miny;
//			Rect roi(minX, minY, maxX-minX, maxY-minY);
//			Mat roi_label = label_mask(roi);
//			Mat roi_bw = bw(roi);
//
//			//peopleOut[nPeop++] = roi;
//
//			//Updates the 0 values of roi_label with the ushort_Max values of roi_bw (foreground)
//			bitwise_or(roi_label, roi_bw, roi_label); //0 and ushort_Max values in roi_label
//
//			//Obtain all the pixels (this is for the demo, it will have to return people instead of points)
//			for (int i = minY; i < maxY; i++)
//			{
//				for (int j = minX; j < maxX; j++)
//				{
//					//ushort maskVal = bw.ptr<uchar>(i)[j];
//					ushort maskVal = label_mask.ptr<uchar>(i)[j];
//					if (maskVal == 255)
//					{
//						ushort curretnVal = currentDepth->ptr<ushort>(i)[j];
//						XnPoint3D p;
//						p.X = (XnFloat)j; p.Y = (XnFloat)i; p.Z = (XnFloat)curretnVal;
//
//						//P.z change to metres
//						//Begin Test: Variance reduction
//					//	float depth_m = p.Z/1000;
//					//	p.Z = (int)83333* (0.0539266 *atanf(1.01081*(-0.88976+depth_m))-0.0539266*atan(1.01081*(-0.88976)));
//					//	float var = 18.7*powf(depth_m, 2) - 33.3*depth_m + 33;
//						//p.Z = (int)100*(p.Z/var);
//						//End Test
//
//
//						points2D[cont++] = p; 
//					}
//				}
//			}
//
//		}
//
//		//updates bg and fg pixels of the model
//		//Mat tmp1, tmp2;
//		//tmp1 = ALPHA_FOREGROUND* (*currentDepth);
//		//tmp2 = (1-ALPHA_FOREGROUND)* backgroundModel_img;
//		//cv::add(tmp1, tmp2, backgroundModel_img, label_mask); //add only in the elements of the mask not 0
//
//		//tmp1 = ALPHA_BACKGROUND* (*currentDepth);
//		//tmp2 = (1-ALPHA_BACKGROUND)* backgroundModel_img;
//		//Mat label_mask_I;
//		//cv::bitwise_not(label_mask, label_mask_I);
//
//		//cv::add(tmp1, tmp2, backgroundModel_img, label_mask_I);	
//
//		Mat tmp1, tmp2;
//		tmp1 = 0.5* (*currentDepth);
//		tmp2 = (1-0.5)* backgroundModel_img;
//		cv::add(tmp1, tmp2, backgroundModel_img, bg_missed); //add only in the elements of the mask not 0
//
//
//		//label_mask.copyTo(*mask);
//
//
//	}
//	//return nPoints;
//	return cont;
//}

void BackgroundDepthSubtraction::findBlobs(const cv::Mat &binary, std::vector < std::vector<cv::Point2i> > &blobs)
{
    blobs.clear();

    // Fill the label_image with the blobs
    // 0  - background
    // 1  - unlabelled foreground
    // 2+ - labelled foreground

    cv::Mat label_image;
    binary.convertTo(label_image, CV_32SC1);

    int label_count = 2; // starts at 2 because 0,1 are used already

    for(int y=0; y < label_image.rows; y++) 
	{
        int *row = (int*)label_image.ptr(y);
        for(int x=0; x < label_image.cols; x++) 
		{
            if(row[x] != 1) 
			{
                continue;
            }

            cv::Rect rect;
            cv::floodFill(label_image, cv::Point(x,y), label_count, &rect, 0, 0, 4);

            std::vector <cv::Point2i> blob;

            for(int i=rect.y; i < (rect.y+rect.height); i++) 
			{
                int *row2 = (int*)label_image.ptr(i);
                for(int j=rect.x; j < (rect.x+rect.width); j++) 
				{
                    if(row2[j] != label_count) 
					{
                        continue;
                    }

                    blob.push_back(cv::Point2i(j,i));
                }
            }

            blobs.push_back(blob);

            label_count++;
        }
    }
}


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

			unit_background = Mat::ones(mask->size(), CV_8UC1); 
			backgroundModel_time = Mat::zeros(mask->size(), CV_8UC1); 

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
		Mat bw = Mat::zeros(backgroundModel_img.size(), CV_8UC1);

		ushort* bMdl_data = (ushort*)backgroundModel_img.data;
		int bMdl_step = backgroundModel_img.step/sizeof(ushort);

		uchar* bw_data = bw.data;
		int bw_step = bw.step;

		ushort* crrnt_data = (ushort*)currentDepth->data;
		int crrnt_step = currentDepth->step/sizeof(ushort);

		int bckRows = backgroundModel_img.rows;
		int bckCols = backgroundModel_img.cols;

		//The threshold depends on the quatitation step at different distances
		for (int i = 0; i < bckRows; i++)
		{
			//ushort* ptrBg = bMdl_data + (i*bMdl_step);
			//uchar* ptrBw = bw_data + i*bw_step;
			//ushort* ptrCu = crrnt_data + i*crrnt_step;
			
			for (int j = 0; j < bckCols; j++)
			{
				int depth = (bMdl_data + (i*bMdl_step))[j];
				int currentVal = (crrnt_data + i*crrnt_step)[j];

				//-800: A calibration error leads to points coordinates bigger than MAX_Z. 
				//A more accurate calibration won't required the decrement
				if (currentVal != 0 && currentVal < (ActivityMap_Utils::MAX_Z-500)) 
				{
					int depthMt = depth/1000;
					//Threshold function linearized with two linear functions
					float thresh;
					if (depthMt > 4)
						//thresh = 229*depthMt - 810;
						thresh = 250*depthMt - 510;
					else
						//thresh = 23*depthMt + 14;
						thresh = 120*depthMt + 60;
					
					//Quadratic function
					//float thresh = (20*(powf(depthMt,2)) + 5*depthMt + 200); //Max difference in depth values at different distances
					
					
					if (abs(depth-currentVal) > thresh)
					{
						(bw_data + i*bw_step)[j] = 1;
						XnPoint3D p;
						p.X = (XnFloat)j; p.Y = (XnFloat)i; p.Z = currentVal;
						points2D[cont++] = p; 
					}
				}
			}
		}


	/*	Mat m;
		erode(bw, bw, m);
		for (int i = 0; i < bckRows; i++)
		{
			for (int j = 0; j < bckCols; j++)
			{
				if ((bw_data + i*bw.step)[j] == 1)
				{
					XnPoint3D p;
					p.X = (XnFloat)j; p.Y = (XnFloat)i; p.Z = currentDepth->ptr<ushort>(i)[j];
					points2D[cont++] = p; 
				}
			}
		}*/

		//clean the noise rejecting small areas
		//vector<vector<Point>> blobs;
		//findBlobs(bw, blobs);
		//
		//vector<vector<Point2i>>::iterator iter = blobs.begin();
		//
		//while (iter != blobs.end())
		//{
		//	vector<Point2i> b = *iter;
		//	if (b.size() > 60) //rejection of small regions
		//	{
		//		for (int i = 0; i < b.size(); i++)
		//		{
		//			Point2i p = b[i];	
		//			XnPoint3D p3D;
		//			p3D.X = p.x; p3D.Y = p.y; p3D.Z = currentDepth->ptr<ushort>(p.y)[p.x];
		//			points2D[cont++] = p3D; 
		//		}
		//	}
		//	iter++;
		//}
		
	


		//Check static foreground pixels
		//cv::add(unit_background, backgroundModel_time, backgroundModel_time, bw); //add unity to the foreground pixels
		//Mat bwNot;
		//bitwise_xor(bw, unit_background, bwNot);
		//threshold(bwNot, bwNot, 1, 1, CV_THRESH_BINARY_INV);
		//cv::subtract(backgroundModel_time, backgroundModel_time, backgroundModel_time, bwNot);// set to 0 the background
		//Mat bg_missed = backgroundModel_time > 10; //bw is type CV_16U

		//Mat tmp1, tmp2;
		//tmp1 = 0.5* (*currentDepth);
		//tmp2 = (1-0.5)* backgroundModel_img;
		//cv::add(tmp1, tmp2, backgroundModel_img, bg_missed); //add only in the elements of the mask not 0
	}
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
void BackgroundDepthSubtraction::subtraction(Mat* currentDepth, Mat* mask, ForegroundObjs* peopleOut)
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
