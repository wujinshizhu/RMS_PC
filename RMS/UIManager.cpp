#include "UIManager.h"

UIManager::UIManager()
{
	camera = new Camera();
	windowName = WNDNAME;
	processor = new Processor();
	cvNamedWindow(windowName); //define the window name
}

void UIManager::ShowVideo(const int timeInterval)
{
	while (1)
	{
		cvShowImage(windowName, camera->QueryFream());
		//stop the video with keyboard action.We'll bind it with buttion click event in the fut
		char set = 's';
		if (set == cvWaitKey(timeInterval))
		{
			break;
		}
	}
}
//used to send image to the camera processor
VOID   CALLBACK  UIManager::TimerProc(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime)
{
	IplImage* frame=UIManager::GetInstance().camera->QueryFream();
	//post Message to processor
	PostThreadMessage(UIManager::GetInstance().processor->GetProcessorId(), UM_WORK, (WPARAM)frame, NULL);

}

void UIManager::StartTimer()
{
	SetTimer(NULL, 1, 1000, TimerProc);
}

UIManager::~UIManager()
{
	KillTimer(NULL, 1);

	cvDestroyWindow(windowName);
	delete camera;
	if (camera != NULL)
	{
		camera = NULL;
	}
}

UIManager & UIManager::GetInstance()
{
	static UIManager instance;   //唯一单例实体
	return instance;
}
