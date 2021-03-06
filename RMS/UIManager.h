#pragma once
#include "Camera.h"
#include <windows.h>
#include <stdio.h>
#include "Processor.h"
#include "RMS_Socket.h"
#include "Config.h"
#define  WNDNAME "camera"

//used to manager the way we show the data
class UIManager
{
private:
	//singleton model
	Camera *camera;
	const char * windowName;
	Processor * processor;
	RMS_Socket * socket;
	Config * config;
	DWORD timerId;
	UIManager();
	//used to ban the copy constructor to ensure singleton
	UIManager(const UIManager&);
	UIManager & operator = (const UIManager&);

public:

	//show the video we get from the camera
	void ShowVideo(const int timeInterval = 30);
    void StartTimer();
	void ResetTimer();
	virtual ~UIManager();
	//used to send image to the camera processor
	static VOID   CALLBACK  TimerProc(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime);
	void sendSocketIdToProcessor();
	//static UIManager* GetInstance();
	static UIManager & GetInstance();
	//������Ϣ
	void ProcessMessage();
	
};


