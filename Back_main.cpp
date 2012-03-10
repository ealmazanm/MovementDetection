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

int main()
{
	CameraProperties cam;
	//Utils::rgbdInit(&cam, 1);
	cam.getContext()->Init();
	cam.getContext()->OpenFileRecording("D:\\pruebaKinect.oni");
	cam.getContext()->FindExistingNode(XN_NODE_TYPE_DEPTH, *(cam.getDepthNode()));

	char* windName_Depth = "Depth image";
	char* windName_Back = "Background subtraction";
	char* windName_BackModel = "Background Model";
	IplImage* depthImg = cvCreateImageHeader(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 3);
	IplImage* backImg = cvCreateImage(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 1);
	IplImage* backImg_model = cvCreateImageHeader(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 3);
	cvNamedWindow(windName_Depth);
	cvNamedWindow(windName_Back);
	cvNamedWindow(windName_BackModel);

	
	cam.getContext()->StartGeneratingAll();
	cam.getContext()->WaitAndUpdateAll();
	BackgroundDepthSubtraction subtractor(cam.getDepthNode()->GetDepthMap());
	bool stop = false;
	unsigned short depth[MAX_DEPTH];
	unsigned short depth_back[MAX_DEPTH];
	char *depth_data, *depth_data_back;
	vector<XnPoint3D> points2D;
	while (!stop)
	{
		cam.getContext()->WaitAndUpdateAll();
		Utils::initImage(backImg, 0);
		const XnDepthPixel* dm = cam.getDepthNode()->GetDepthMap();
		subtractor.subtraction(&points2D, dm);
		BackgroundDepthSubtraction::createBackImage(&points2D, backImg);

		const XnDepthPixel* backDm = subtractor.getBackgroundDepth();

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
	
	cam.getContext()->StopGeneratingAll();
	cvReleaseImageHeader(&depthImg);
	cvReleaseImage(&backImg);
	cvReleaseImageHeader(&backImg_model);
	cam.getContext()->Shutdown();

	return 0;
}