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
	//���ڿ������ϵ����⣬����processor�޷����RMS_socket�̵߳�ID����������������̶߳����ߵ���֪�ԣ�����RMS_socket��ID��Process
	//�Ӷ�processor ������RMS_socket����ͨ��
	UIManager::GetInstance().sendSocketIdToProcessor();
	//UIManager::GetInstance().StartSocketService();
	UIManager::GetInstance().ShowVideo();
	//system("pause");
	/*
	while(cvWaitKey(100) != 's')
	{
		cvWaitKey(100);
	}
	*/
	return 0;
	//�������
	

}


