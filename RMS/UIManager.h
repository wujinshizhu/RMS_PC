#pragma once
#include "Camera.h"
#include <windows.h>
#include <stdio.h>
#include "Processor.h"

#define  WNDNAME "camera"
#define  UM_WORK   (WM_USER+1002)

//used to manager the way we show the data
class UIManager
{
private:
	//singleton model
	Camera *camera;
	const char * windowName;
	Processor * processor;
	UIManager();
	//used to ban the copy constructor to ensure singleton
	UIManager(const UIManager&);
	UIManager & operator = (const UIManager&);

public:
	
	//show the video we get from the camera
	void ShowVideo(const int timeInterval = 33);
    void StartTimer();
	virtual ~UIManager();
	//used to send image to the camera processor
	static VOID   CALLBACK  TimerProc(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime);
	//static UIManager* GetInstance();
	static UIManager & GetInstance();
};


