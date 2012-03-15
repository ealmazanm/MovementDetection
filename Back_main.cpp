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

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <boost/date_time/posix_time/ptime.hpp>

using namespace boost::posix_time;

/*
Three different types of background subtraction method
 1.- Using depth information.
 2.- Using color information.
 3.- Using a combinagion of depth and color.
*/
const int BACKGROUND_SUB_TYPE = 1;
const int PERCENTAGE_OF_POINTS = 15;

ofstream outDebug(filePaths::DEBUG_FILEPATH, ios::out);


void depthBackgroundSub(CameraProperties* cam)
{
	//char* windName_Depth = "Depth image";
	char* windName_Back = "Background subtraction";
	//IplImage* depthImg = cvCreateImageHeader(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 3);
	IplImage* backImg = cvCreateImage(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 1);
	//cvNamedWindow(windName_Depth);
	cvNamedWindow(windName_Back);

	cam->getContext()->StartGeneratingAll();

	bool stop = false;
	bool firstTime = true;

	int total = XN_VGA_Y_RES*XN_VGA_X_RES;
	BackgroundSubtraction_factory* subtractor;
	//allocate enough memory in advance (15% of the total points)
	XnPoint3D* points2D = new XnPoint3D[PERCENTAGE_OF_POINTS*total/100];	
	int numPoints = 0;
	int contFrames = 0;

	while (!stop)
	{
		

		cam->getContext()->WaitAndUpdateAll();
	
		const XnDepthPixel* dm = cam->getDepthNode()->GetDepthMap();

		ptime time_start_wait(microsec_clock::local_time());
		if (contFrames == 0)
			subtractor = new BackgroundDepthSubtraction(dm);
		else
			numPoints = subtractor->subtraction(points2D, dm); //returns the num poins of foreground
		
		
		
		Utils::initImage(backImg, 0);
		subtractor->createBackImage(points2D, backImg, numPoints);

		//create depth image
		//depth_data = new char[640*480*3];
		//Utils::raw2depth(depth, MAX_DEPTH);
		//Utils::depth2rgb(dm, depth, depth_data);
		//cvSetData(depthImg, depth_data, 640*3);	

		//display images
	//	cvShowImage(windName_Depth, depthImg);
		cvShowImage(windName_Back, backImg);


	////	delete(depth_data);
		char c = cvWaitKey(1);

		ptime time_end_wait(microsec_clock::local_time());
		time_duration duration_wait(time_end_wait - time_start_wait);
		outDebug << "Time report(bgs+show): " << duration_wait.total_microseconds() << endl;



	//	stop = (c == 27);

		stop = cam->getDepthNode()->GetFrameID() == 350;

		if (cam->getDepthNode()->GetFrameID() == 1)
			if (firstTime ? firstTime = false : stop = true);

		contFrames++;
	}
	//ptime time_end(microsec_clock::local_time());
	//time_duration duration(time_end - time_start);
	//double totalSecs = duration.total_microseconds()/1000000;
	//double fps = contFrames/totalSecs;
	//cout << "Fps: " << fps << endl;

	cam->getContext()->StopGeneratingAll();
	cam->getContext()->Shutdown();

	//free memory
	cvReleaseImage(&backImg);
	//cvReleaseImageHeader(&depthImg);
	cvDestroyAllWindows();
	delete(points2D);
}


void rgbdBackgroundSub(CameraProperties* cam)
{
}

void rgbBackgroundSub(CameraProperties* cam)
{
}

int main()
{
	CameraProperties cam;
	Utils::rgbdInit(&cam, 1);
	//cam.getContext()->Init();
	//cam.getContext()->OpenFileRecording("D:\\pruebaKinect.oni");
	//cam.getContext()->FindExistingNode(XN_NODE_TYPE_DEPTH, *(cam.getDepthNode()));
	//cam.getContext()->FindExistingNode(XN_NODE_TYPE_IMAGE, *(cam.getImageNode()));

	switch (BACKGROUND_SUB_TYPE)
	{
	case 1: //Depth
		{
			//depthBackgrounSub_Memory(&cam);
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