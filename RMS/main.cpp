//#include <opencv2\opencv.hpp> //opencv2下是C++接口
#include <opencv\cxcore.h>
//#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <process.h>
#include <stdio.h>
#include "Camera.h"
#include "UIManager.h"
#include "Processor.h"
using namespace std;
using namespace cv;//cv是Opencv的名字空间


int main()
{
	UIManager::GetInstance().StartTimer();
	//由于框架设计上的问题，导致processor无法获得RMS_socket线程的ID，因而采用利用主线程对两者的已知性，发送RMS_socket的ID给Process
	//从而processor 可以与RMS_socket进行通信
	UIManager::GetInstance().sendSocketIdToProcessor();
	//UIManager::GetInstance().StartSocketService();
	UIManager::GetInstance().ShowVideo();
	return 0;
	//程序结束
	

}


