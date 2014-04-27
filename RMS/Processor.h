#pragma once
#include <windows.h>
#include <opencv\highgui.h>
#include <opencv2\core\core.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\legacy\legacy.hpp>
#include <opencv2\imgproc\imgproc.hpp>


#include <iostream>
using namespace cv;
using namespace std;


#define  UM_WORK   (WM_USER+1002)

class Processor
{
private:
	HANDLE processorHnd;
	DWORD processorID;
	
public:

	Processor();
	static DWORD WINAPI ProcessFrame(LPVOID lpParam);
	static void CompareImage(IplImage *image1, IplImage *image2);
	DWORD GetProcessorId();
	static void SavePicture(IplImage *image);
	virtual ~Processor();
};

