#include "UIManager.h"

UIManager::UIManager()
{
	camera = new Camera();
	windowName = WNDNAME;
	config = new Config();
	socket = new RMS_Socket();
	processor = new Processor(config->GetThreshold());
	
}

void UIManager::ShowVideo(const int timeInterval)
{
	cvNamedWindow(windowName); //define the window name
	while (1)
	{
		cvShowImage(windowName, camera->QueryFream());
		//接收控制信息	
		ProcessMessage();
		char set = 's';
		if (set == cvWaitKey(timeInterval))
		{
			break;
		}
	}
	cvDestroyWindow(windowName);
}

void UIManager::ProcessMessage()
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
		int sample;
		int threshold;
		int recoverTime;
		switch (msg.message)
		{
		case UM_CONTORL_SAMPLEINTERVAL:
			sample = msg.wParam;
			config->SetSampleInterval(sample);
			config->Save();
			ResetTimer();
			break;
		case UM_CONTORL_THRESHOLD:
			threshold = msg.wParam;
			config->SetThreshold(threshold);
			config->Save();
			processor->SetThreshold(threshold);
			break;
		case UM_CONTORL_RECOVERTIME:
			recoverTime = msg.wParam;
			config->SetRecoverTime(recoverTime);
			config->Save();
			printf("异常回复时间变更为%d分钟\n", config->GetRecoverTime());
			break;
		case UM_TRUST:
			//停止图像的获取
			KillTimer(NULL, timerId);
			printf("服务器进入休眠状态，休眠时间为%d分钟\n", config->GetRecoverTime());
			//启动休眠计时器
			Sleep(config->GetRecoverTime() * 60 * 1000);
			//重新启动监控
			timerId = SetTimer(NULL, 1, config->GetSampleInterval() * 1000, TimerProc);
			break;
		default:
			break;
		}
	}
}
//used to send image to the camera processor
VOID   CALLBACK  UIManager::TimerProc(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime)
{
	IplImage* frame=UIManager::GetInstance().camera->QueryFream();
	//post Message to processor
	if (frame != NULL)
	{
		PostThreadMessage(UIManager::GetInstance().processor->GetProcessorId(), UM_WORK, (WPARAM)frame, NULL);
	}
}
/*
void UIManager::sendSocketIdToProcessor()
{
	//send the threadID of socket to processor to communication
	PostThreadMessage(UIManager::GetInstance().processor->GetProcessorId(), UM_ID,
		(WPARAM)UIManager::GetInstance().socket->GetThreadID(), NULL);
}
*/

void UIManager::StartTimer()
{
	timerId=SetTimer(NULL, 1, config->GetSampleInterval() * 1000, TimerProc);
}
void UIManager::ResetTimer()
{
	KillTimer(NULL, timerId);
	printf("采样频率变更为%d秒\n", config->GetSampleInterval());
	timerId = SetTimer(NULL, 1, config->GetSampleInterval() * 1000, TimerProc);
}

UIManager::~UIManager()
{
	KillTimer(NULL, 1);
	delete camera;
	if (camera != NULL)
	{
		camera = NULL;
	}
	delete processor;
	if (processor != NULL)
	{
		processor = NULL;
	}
	delete socket;
	if (socket != NULL)
	{
		socket = NULL;
	}
	delete config;
	if (config != NULL)
	{
		config = NULL;
	}
}

UIManager & UIManager::GetInstance()
{
	static UIManager instance;   //唯一单例实体
	return instance;
}
