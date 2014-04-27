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
	//UIManager::GetInstance().ShowVideo();
	UIManager::GetInstance().StartTimer();
	//_beginthread(processor, 0, NULL);
	while (cvWaitKey(100) != 's')
	{
		cvWaitKey(100);
	}
	//_endthread();

	return 0;
	//程序结束
	

}


