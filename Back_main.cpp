/**
* Back_main.cpp: Main class to test the background subtraction.
*
* Take images from Kinect and perform a background subtraction. Shows the 
* the results in a binary image.
* Compare the results usign threads for more than one camera.
*
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
#include <boost/thread.hpp>

using namespace boost::posix_time;

const int PERCENTAGE_OF_POINTS = 80;
const bool PARALLEL_PROCESS = true;


//Perform a background subtraction in two cameras in a sequencial way
void depthBackgroundSub_Seq(CameraProperties* cam1,  CameraProperties* cam2, ofstream* outDebug)
{
	char* windName_1 = "BackgroundSub 1";
	char* windName_2 = "BackgroundSub 2";
	IplImage* backImg1 = cvCreateImage(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 1);
	IplImage* backImg2 = cvCreateImage(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 1);
	cvNamedWindow(windName_1);
	cvNamedWindow(windName_2);

	cam1->getContext()->StartGeneratingAll();
	cam2->getContext()->StartGeneratingAll();

	bool stop = false;
	bool firstTime = true;

	int total = XN_VGA_Y_RES*XN_VGA_X_RES;
	BackgroundSubtraction_factory *subtractor1, *subtractor2;
	//allocate enough memory in advance (% of the total points)
	XnPoint3D* points2D_1 = new XnPoint3D[MAX_FORGROUND_POINTS];	
	XnPoint3D* points2D_2 = new XnPoint3D[MAX_FORGROUND_POINTS];	
	int numPoints_1 = 0;
	int numPoints_2 = 0;
	int contFrames = 0;

	unsigned short depth[MAX_DEPTH];
	unsigned short depth2[MAX_DEPTH];
	char *depth_data, *depth_data2;
	while (!stop)
	{
		//wait for the next frame to be ready
		cam1->getContext()->WaitAndUpdateAll();
		cam2->getContext()->WaitAndUpdateAll();
		//recover the depth map
		const XnDepthPixel* dm1 = cam1->getDepthNode()->GetDepthMap();
		const XnDepthPixel* dm2 = cam2->getDepthNode()->GetDepthMap();


		//ptime time_start_wait(microsec_clock::local_time());
		if (contFrames == 0)//store the background model
		{
			subtractor1 = new BackgroundDepthSubtraction(dm1);
			subtractor2 = new BackgroundDepthSubtraction(dm2);
		}
		else 
		{
			numPoints_1 = subtractor1->subtraction(points2D_1, dm1); //returns the num poins of foreground
			numPoints_2 = subtractor2->subtraction(points2D_2, dm2); //returns the num poins of foreground
		}
		
		//ptime time_end_wait(microsec_clock::local_time());
		//time_duration duration_wait(time_end_wait - time_start_wait);
		//(*outDebug) << "Time report(bgs 1_2): " << duration_wait.total_microseconds() << endl;
				
		//Utils::initImage(backImg1, 0);
		//Utils::initImage(backImg2, 0);
		//subtractor1->createBackImage(points2D_1, backImg1, numPoints_1);
		//subtractor2->createBackImage(points2D_2, backImg2, numPoints_2);

		////display image
		//cvShowImage(windName_1, backImg1);
		//cvShowImage(windName_2, backImg2);
		//char c = cvWaitKey(1);
		//stop = (c == 27) || (cam1->getDepthNode()->GetFrameID() == 250);

		stop = (contFrames == 250);
		
		//for recorded videos
	//	if (cam2->getDepthNode()->GetFrameID() == 1)
	//		if (firstTime ? firstTime = false : stop = true);

		contFrames++;
	}
	//ptime time_end(microsec_clock::local_time());
	//time_duration duration(time_end - time_start);
	//double totalSecs = duration.total_microseconds()/1000000;
	//double fps = contFrames/totalSecs;
	//cout << "Fps: " << fps << endl;

	cam1->getContext()->StopGeneratingAll();
	cam2->getContext()->StopGeneratingAll();

	//free memory
	cvReleaseImage(&backImg1);
	cvReleaseImage(&backImg2);
	cvDestroyAllWindows();
	delete(points2D_1);
	delete(points2D_2);
	delete(subtractor1);
	delete(subtractor2);
}


//Perform a background subtraction of one camera
void depthBackgroundSub_Par(CameraProperties* cam, ofstream* outDebug)
{
	char camId[20];
	_itoa(cam->getCamId(), camId, 10);
	char windName_Back[50];
	strcpy(windName_Back, "Background subtraction ");
	strcat(windName_Back, camId);
	IplImage* backImg = cvCreateImage(cvSize(XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 1);
	cvNamedWindow(windName_Back);

	cam->getContext()->StartGeneratingAll();

	bool stop = false;
	bool firstTime = true;

	int total = XN_VGA_Y_RES*XN_VGA_X_RES;
	BackgroundSubtraction_factory* subtractor;
	//allocate enough memory in advance (% of the total points)
	XnPoint3D* points2D = new XnPoint3D[MAX_FORGROUND_POINTS];	
	int numPoints = 0;
	int contFrames = 0;

	while (!stop)
	{
		//wait for the next frame to be ready
		cam->getContext()->WaitAndUpdateAll();
		//recover the depth map
		const XnDepthPixel* dm = cam->getDepthNode()->GetDepthMap();

		//ptime time_start_wait(microsec_clock::local_time());
		if (contFrames == 0)//store the background model
			subtractor = new BackgroundDepthSubtraction(dm);
		else 
			numPoints = subtractor->subtraction(points2D, dm); //returns the num poins of foreground
		
		//ptime time_end_wait(microsec_clock::local_time());
		//time_duration duration_wait(time_end_wait - time_start_wait);
		//(*outDebug) << "Time report(bgs "<< camId << "): " << duration_wait.total_microseconds() << endl;
				
		Utils::initImage(backImg, 0);
		subtractor->createBackImage(points2D, backImg, numPoints);

		//display image
		cvShowImage(windName_Back, backImg);
		char c = cvWaitKey(1);
		stop = (c == 27) || (cam->getDepthNode()->GetFrameID() == 250);
		
//		stop = (contFrames == 250);
		//for recorded videos
	//	if (cam->getDepthNode()->GetFrameID() == 1)
	//		if (firstTime ? firstTime = false : stop = true);

		contFrames++;
	}
	//ptime time_end(microsec_clock::local_time());
	//time_duration duration(time_end - time_start);
	//double totalSecs = duration.total_microseconds()/1000000;
	//double fps = contFrames/totalSecs;
	//cout << "Fps: " << fps << endl;

	cam->getContext()->StopGeneratingAll();

	//free memory
	cvReleaseImage(&backImg);
	cvDestroyAllWindows();
	delete(points2D);
	delete(subtractor);
}



int main()
{
	ofstream outDebug1(filePaths::DEBUG_FILEPATH, ios::out);
	ofstream outDebug2("D:/CameraCalibrations/debug2.txt", ios::out);

	//One camera perform the background subtraction on live
	//and the other perform a backgground subtraction on a loaded video.
	CameraProperties cam1, cam2;
//	Utils::rgbdInit(&cam1, &cam2);
	Utils::rgbdInit(&cam1, 1);
	cam2.setCamId(2);
	cam2.getContext()->Init();
	cam2.getContext()->OpenFileRecording("D:\\pruebaKinect.oni");
	cam2.getContext()->FindExistingNode(XN_NODE_TYPE_DEPTH, *(cam2.getDepthNode()));
	cam2.getContext()->FindExistingNode(XN_NODE_TYPE_IMAGE, *(cam2.getImageNode()));

	ptime time_start_wait(microsec_clock::local_time());
	if (PARALLEL_PROCESS) //both cameras in parallel
	{
		boost::thread thr(depthBackgroundSub_Par, &cam2, &outDebug2);
		depthBackgroundSub_Par(&cam1, &outDebug1);
		thr.join();
	}
	else //both cameras in sequence
	{
		depthBackgroundSub_Seq(&cam1, &cam2, &outDebug1);
	}
	//Take time
	ptime time_end_wait(microsec_clock::local_time());
	time_duration duration_wait(time_end_wait - time_start_wait);
	outDebug1 << "Time report(bgs 1_2): " << duration_wait.total_microseconds() << endl;


	cam1.getContext()->Shutdown();
	cam2.getContext()->Shutdown();

	return 0;
}