#pragma once
#include <opencv\cxcore.h>
//#include <opencv\cv.h>
#include <opencv\highgui.h>
class Camera
{
private:
	//used to control the camera
	CvCapture * CameraCapture;
	//used to get the frame of the camera
	IplImage* frame;
public:
	//using 0 as the default cameraId
	Camera(const int cameraId = 0);
	IplImage* QueryFream();
	virtual ~Camera();
};

