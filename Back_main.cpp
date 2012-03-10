/**
* Back_main.cpp: Main class to test the background subtraction.
*
* Take images from Kinect and perform a background subtraction. Shows the 
* the results in a binary image.
*
* Libraries: OpenNI, OpenCV.
*
* Author: Emilio J. Almazan <emilio.almazan@kingston.ac.uk>, 2012
*/
#include "BackgroundDepthSubtraction.h"
#include "BackgroundColorSubtraction.h"


/*
Three different types of background subtraction method
 1.- Using depth information.
 2.- Using color information.
 3.- Using a combinagion of depth and color.
*/
const int BACKGROUND_SUB_TYPE = 1;

void depthBackgroundSub(CameraProperties* cam)
{
	char* windName_Depth = "Depth image";
	char* windName_Back = "Background subtraction";
	char* windName_BackModel = "Background Model";
	IplImage* depthImg = cvCreateImageHeader(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 3);
	IplImage* backImg = cvCreateImage(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 1);
	IplImage* backImg_model = cvCreateImageHeader(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 3);
	cvNamedWindow(windName_Depth);
	cvNamedWindow(windName_Back);
	cvNamedWindow(windName_BackModel);


	cam->getContext()->StartGeneratingAll();
	cam->getContext()->WaitAndUpdateAll();
	BackgroundSubtraction_factory* subtractor = new BackgroundDepthSubtraction(cam->getDepthNode()->GetDepthMap());
	bool stop = false;
	unsigned short depth[MAX_DEPTH];
	unsigned short depth_back[MAX_DEPTH];
	char *depth_data, *depth_data_back;
	vector<XnPoint3D> points2D;
	while (!stop)
	{
		cam->getContext()->WaitAndUpdateAll();
		Utils::initImage(backImg, 0);
		const XnDepthPixel* dm = cam->getDepthNode()->GetDepthMap();
		subtractor->subtraction(&points2D, dm);
		subtractor->createBackImage(&points2D, backImg);

		const XnDepthPixel* backDm = (const XnDepthPixel*)subtractor->getBackgroundModel();

		//create depth image
		depth_data_back = new char[640*480*3];
		Utils::raw2depth(depth_back, MAX_DEPTH);
		Utils::depth2rgb(backDm, depth_back, depth_data_back);
		cvSetData(backImg_model, depth_data_back, 640*3);	

		depth_data = new char[640*480*3];
		Utils::raw2depth(depth, MAX_DEPTH);
		Utils::depth2rgb(dm, depth, depth_data);
		cvSetData(depthImg, depth_data, 640*3);	

		//display images
		cvShowImage(windName_Depth, depthImg);
		cvShowImage(windName_Back, backImg);
		cvShowImage(windName_BackModel, backImg_model);

		delete(depth_data);
		delete(depth_data_back);
		points2D.clear();
		char c = cvWaitKey(1);
		stop = (c == 27);
	}

	cam->getContext()->StopGeneratingAll();
	cvReleaseImageHeader(&depthImg);
	cvReleaseImage(&backImg);
	cvReleaseImageHeader(&backImg_model);
}

void rgbBackgroundSub(CameraProperties* cam)
{
	char* windName_RGB = "RGB image";
	char* windName_Back = "Background subtraction";
	char* windName_BackModel = "Background Model";
	cvNamedWindow(windName_RGB);
	cvNamedWindow(windName_Back);
	cvNamedWindow(windName_BackModel);

	IplImage* subtractImg = cvCreateImage(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 1);
	IplImage* currentImg_RGB = cvCreateImage(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 3);
	IplImage* currentImg_Gray = cvCreateImage(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 1);
	
	cam->getContext()->StartGeneratingAll();
	//Take the background model
	cam->getContext()->WaitAndUpdateAll();
	const XnRGB24Pixel* rgbMap = cam->getImageNode()->GetRGB24ImageMap();
	Utils::fillImageDataFull(currentImg_RGB, rgbMap);
	cvConvertImage(currentImg_RGB, currentImg_Gray, CV_BGR2GRAY);
	BackgroundSubtraction_factory* subtractor = new BackgroundColorSubtraction(currentImg_Gray);
	bool stop = false;
	vector<XnPoint3D> points2D;
	while (!stop)
	{
		cam->getContext()->WaitAndUpdateAll();
		Utils::initImage(subtractImg, 0);
		rgbMap = cam->getImageNode()->GetRGB24ImageMap();
		Utils::fillImageDataFull(currentImg_RGB, rgbMap);
		cvConvertImage(currentImg_RGB, currentImg_Gray, CV_BGR2GRAY);
		subtractor->subtraction(&points2D, currentImg_Gray);
		subtractor->createBackImage(&points2D, subtractImg);

		const IplImage* backDm = (const IplImage*)subtractor->getBackgroundModel();

		//display images
		cvShowImage(windName_RGB, subtractImg);
		cvShowImage(windName_Back, currentImg_RGB);
		cvShowImage(windName_BackModel, backDm);

		points2D.clear();
		char c = cvWaitKey(1);
		stop = (c == 27);
	}
	
	cam->getContext()->StopGeneratingAll();
	cvReleaseImage(&currentImg_RGB);
	cvReleaseImage(&subtractImg);
	cvReleaseImage(&currentImg_Gray);

}

void rgbdBackgroundSub(CameraProperties* cam)
{
}


int main()
{
	CameraProperties cam;
	//Utils::rgbdInit(&cam, 1);
	cam.getContext()->Init();
	cam.getContext()->OpenFileRecording("D:\\pruebaKinect.oni");
	cam.getContext()->FindExistingNode(XN_NODE_TYPE_DEPTH, *(cam.getDepthNode()));
	cam.getContext()->FindExistingNode(XN_NODE_TYPE_IMAGE, *(cam.getImageNode()));

	switch (BACKGROUND_SUB_TYPE)
	{
	case 1: //Depth
		{
			depthBackgroundSub(&cam);
			break;
		}
	case 2: //Color
		{
			rgbBackgroundSub(&cam);
			break;
		}
	case 3: //Depth + Color
		{
			rgbdBackgroundSub(&cam);
			break;
		}
	default:
		{
			cout << "Incorrect Background subtraction method!! " << endl; 
		}
	}

	cam.getContext()->Shutdown();

	return 0;
}