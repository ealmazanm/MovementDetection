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

const int PERCENTAGE_OF_POINTS = 80;
const bool PARALLEL_PROCESS = true;


//Perform a background subtraction in two cameras in a sequencial way
void depthBackgroundSub_Seq(KinectSensor* cam1,  KinectSensor* cam2)
{
	char* windName_1 = "BackgroundSub 1";
	char* windName_2 = "BackgroundSub 2";
	Mat backImg1(Size(XN_VGA_X_RES, XN_VGA_Y_RES), CV_8UC1);
	Mat backImg2(Size(XN_VGA_X_RES, XN_VGA_Y_RES), CV_8UC1);
	namedWindow(windName_1);
	namedWindow(windName_2);

	cam1->startDevice();
	cam2->startDevice();

	bool stop = false;
	bool firstTime = true;

	int total = XN_VGA_Y_RES*XN_VGA_X_RES;
	BackgroundDepthSubtraction *subtractor1, *subtractor2;
	//allocate enough memory in advance (% of the total points)
	XnPoint3D* points2D_1 = new XnPoint3D[MAX_FORGROUND_POINTS];	
	XnPoint3D* points2D_2 = new XnPoint3D[MAX_FORGROUND_POINTS];	
	int numPoints_1 = 0;
	int numPoints_2 = 0;
	int contFrames = 0;

//	unsigned short depth[MAX_DEPTH];
//	unsigned short depth2[MAX_DEPTH];
	char *depth_data, *depth_data2;
	while (!stop)
	{
		//wait for the next frame to be ready
		cam1->waitAndUpdate();
		cam2->waitAndUpdate();
		//recover the depth map
		const XnDepthPixel* dm1 = cam1->getDepthMap();
		const XnDepthPixel* dm2 = cam2->getDepthMap();


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
		
		Utils::initMat1u(backImg1, 0);
		Utils::initMat1u(backImg2, 0);
		subtractor1->createBackImage(points2D_1, backImg1, numPoints_1);
		subtractor2->createBackImage(points2D_2, backImg2, numPoints_2);

		imshow(windName_1, backImg1);
		imshow(windName_2, backImg2);
		////display image
		char c = cvWaitKey(1);
		stop = (c == 27) || (contFrames == 250);

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

	cam1->stopDevice();
	cam2->stopDevice();

	//free memory
	delete(points2D_1);
	delete(points2D_2);
	delete(subtractor1);
	delete(subtractor2);
}


//Perform a background subtraction of one camera
void depthBackgroundSub_Par(KinectSensor* cam, ofstream* outDebug)
{
	char camId[20];
	_itoa(cam->getIdCam(), camId, 10);
	char windName_Back[50];
	strcpy(windName_Back, "Background subtraction ");
	strcat(windName_Back, camId);
	Mat backImg(Size(XN_VGA_X_RES, XN_VGA_Y_RES), CV_8UC1);
	namedWindow(windName_Back);

	cam->startDevice();

	bool stop = false;
	bool firstTime = true;

	int total = XN_VGA_Y_RES*XN_VGA_X_RES;
	BackgroundDepthSubtraction* subtractor;
	//allocate enough memory in advance (% of the total points)
	XnPoint3D* points2D = new XnPoint3D[MAX_FORGROUND_POINTS];	
	int numPoints = 0;
	int contFrames = 0;

	while (!stop)
	{
		//wait for the next frame to be ready
		cam->waitAndUpdate();
		//recover the depth map
		const XnDepthPixel* dm = cam->getDepthMap();

		//ptime time_start_wait(microsec_clock::local_time());
		if (contFrames == 0)//store the background model
			subtractor = new BackgroundDepthSubtraction(dm);
		else 
			numPoints = subtractor->subtraction(points2D, dm); //returns the num poins of foreground
		
		//ptime time_end_wait(microsec_clock::local_time());
		//time_duration duration_wait(time_end_wait - time_start_wait);
		//(*outDebug) << "Time report(bgs "<< camId << "): " << duration_wait.total_microseconds() << endl;
				
		Utils::initMat1u(backImg, 0);
		subtractor->createBackImage(points2D, backImg, numPoints);

		//display image
		imshow(windName_Back, backImg);
		char c = cvWaitKey(1);
		stop = (c == 27) || (contFrames == 250);
		
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

	cam->stopDevice();

	//free memory
	delete(points2D);
	delete(subtractor);
}



int main()
{
	//ofstream outDebug1(filePaths::DEBUG_FILEPATH, ios::out);
	//ofstream outDebug2("D:/CameraCalibrations/debug2.txt", ios::out);

	//One camera perform the background subtraction on live
	//and the other perform a backgground subtraction on a loaded video.
	KinectSensor cam1, cam2;
//	Utils::rgbdInit(&cam1, &cam2);
	cam1.initDevice(1,2,true);
	cam2.initDevice(2,2,true);
	//cam2.setCamId(2);
	//cam2.getContext()->Init();
	//cam2.getContext()->OpenFileRecording("D:\\pruebaKinect.oni");
	//cam2.getContext()->FindExistingNode(XN_NODE_TYPE_DEPTH, *(cam2.getDepthNode()));
	//cam2.getContext()->FindExistingNode(XN_NODE_TYPE_IMAGE, *(cam2.getImageNode()));

	depthBackgroundSub_Seq(&cam1, &cam2);
	
	//Take time
	cam1.shutDown();
	cam2.shutDown();

	return 0;
}