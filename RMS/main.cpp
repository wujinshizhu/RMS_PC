//#include <opencv2\opencv.hpp> //opencv2����C++�ӿ�
#include <opencv\cxcore.h>
//#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <process.h>
#include <stdio.h>
#include "Camera.h"
#include "UIManager.h"
#include "Processor.h"
using namespace std;
using namespace cv;//cv��Opencv�����ֿռ�


int main()
{
	UIManager::GetInstance().StartTimer();
	//UIManager::GetInstance().StartSocketService();
	UIManager::GetInstance().ShowVideo();
	return 0;
	//�������
	

}


