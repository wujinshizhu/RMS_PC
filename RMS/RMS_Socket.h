#pragma once
#include <winsock.h>
#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>

#pragma comment(lib, "WS2_32")	// 链接到WS2_32.lib
#define PORT 20000
#define TIME_OVER 500000
#include "Processor.h"

class RMS_Socket
{
private:
	//用于加载winsock库
	WSADATA wsa;

	//用于管理socket通信的线程
	HANDLE sockThreadHnd;
	//线程ID
	static DWORD sockThreadId;
	static DWORD mainThreadId;

	enum type:char
	{
		heartbeat,//type=0时，表示心跳信息，用于检测目标手机是否在线
		picture,//type=1时，表示传输图片
		video,//type=2时，表示传输视频，视时间是否充足决定是否实现
		control,//type=3时，表示传递控制信息
		disconnect,//type=4时，表示断开连接
		trust//type=5时，表示信任当前异常
	};
	enum ControlType:char
	{
		sampleInterval,//采样频率
		threshold,//异常检测阈值
		recoverTime//异常恢复时间
	};
	bool static isConnectted;
	
public:
	RMS_Socket();
	//开始进行socket通信
	void Start();
	static void sendFile(const SOCKET &sockConn, IplImage* frame);
	static void sendFileWithoutDetect(const SOCKET &sockConn, IplImage* frame);
	virtual ~RMS_Socket();
	//用于管理通信
	static DWORD WINAPI CommManager(LPVOID lpParam);
	static DWORD GetThreadID();
	//处理本地的消息队列，主要是processor发来的消息
	static void ProcessMessage(SOCKET sockConn);
	//用于本地的小端Int数据转换为大端int
	static int EndianConvertLToB(int);
	//大端转换为小端
	static int EndianConvertBToL(int);
	//用于接受手机端发来的请求
	static void ReceviceRequest(SOCKET sockConn,int &timerId);
	static DWORD GetMainThreadId(DWORD processId = 0);
	static VOID   CALLBACK  TimerProc(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime);
};

