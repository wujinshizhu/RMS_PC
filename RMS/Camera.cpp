#include "Camera.h"
#include <windows.h>

Camera::Camera(const int cameraId)
{
	CameraCapture = cvCaptureFromCAM(cameraId);
	if (!CameraCapture)
	{
		MessageBox(NULL, "we can find the camera,please make sure again", "Warn", MB_OK);
	}
	frame = NULL;
}

IplImage* Camera::QueryFream()
{
	frame = cvQueryFrame(CameraCapture);
	return frame;
}


Camera::~Camera()
{
	cvReleaseCapture(&CameraCapture);
	if(CameraCapture != NULL)
	{
		CameraCapture = NULL;
	}
}

