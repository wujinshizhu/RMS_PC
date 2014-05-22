#pragma once
#include <winsock.h>
#include <iostream>
#pragma comment(lib, "WS2_32")	// 链接到WS2_32.lib
#define PORT 20000
#define TIME_OVER 5000
#include "Processor.h"
class RMS_Socket
{
private:
	//用于加载winsock库
	WSADATA wsa;

	//用于管理socket通信的线程
	HANDLE sockThreadHnd;
	//线程ID
	DWORD sockThreadId;

	enum type:char
	{
		heartbeat,//type=0时，表示心跳信息，用于检测目标手机是否在线
		picture,//type=1时，表示传输图片
		video,//type=2时，表示传输视频，视时间是否充足决定是否实现
		control,//type=3时，表示传递控制信息
		disconnect//type=4时，表示断开连接
	};
	bool static isConnectted;
	
public:
	RMS_Socket();
	//开始进行socket通信
	void Start();
	static void sendFile(const SOCKET &sockConn, IplImage* frame);
	virtual ~RMS_Socket();
	//用于管理通信
	static DWORD WINAPI CommManager(LPVOID lpParam);
	DWORD GetThreadID();
	//处理本地的消息队列，主要是processor发来的消息
	static void ProcessMessage(SOCKET sockConn);
	//用于本地的小端Int数据转换为大端int
	static int EndianConvertLToB(int);
	//用于接受手机端发来的请求
	static void ReceviceRequest(SOCKET sockConn,int &timerId);
	static VOID   CALLBACK  TimerProc(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime);
};

