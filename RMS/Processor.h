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
#include <stdio.h>

#include <iostream>
using namespace cv;
using namespace std;

#define  UM_WORK   (WM_USER+1002)
#define  UM_CONTORL_SAMPLEINTERVAL  (WM_USER+1003)
#define  UM_CONTORL_THRESHOLD  (WM_USER+1004)
#define  UM_CONTORL_RECOVERTIME (WM_USER+1005)
#define	 UM_TRUST (WM_USER+1006)
#define PATH "D:\\RMS\\"
#include "RMS_Socket.h"

class Processor
{
private:
	HANDLE processorHnd;
	DWORD processorID;
	static DWORD SockThreadID;
	//用于判断是否发生变动
	static int threshold;
public:

	Processor(int threshold);
	static DWORD WINAPI ProcessFrame(LPVOID lpParam);
	static float CompareImage(IplImage *image1, IplImage *image2);
	DWORD GetProcessorId();
	static void SavePicture(IplImage *image);
	//将检测到的异常图片发送给socket
	static void sendToSocket(IplImage * image,DWORD sockThreadID);
	static void SetThreshold(int);
	virtual ~Processor();
};

