#pragma once
#include <winsock.h>
#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>

#pragma comment(lib, "WS2_32")	// ���ӵ�WS2_32.lib
#define PORT 20000
#define TIME_OVER 500000
#include "Processor.h"

class RMS_Socket
{
private:
	//���ڼ���winsock��
	WSADATA wsa;

	//���ڹ���socketͨ�ŵ��߳�
	HANDLE sockThreadHnd;
	//�߳�ID
	static DWORD sockThreadId;
	static DWORD mainThreadId;

	enum type:char
	{
		heartbeat,//type=0ʱ����ʾ������Ϣ�����ڼ��Ŀ���ֻ��Ƿ�����
		picture,//type=1ʱ����ʾ����ͼƬ
		video,//type=2ʱ����ʾ������Ƶ����ʱ���Ƿ��������Ƿ�ʵ��
		control,//type=3ʱ����ʾ���ݿ�����Ϣ
		disconnect,//type=4ʱ����ʾ�Ͽ�����
		trust//type=5ʱ����ʾ���ε�ǰ�쳣
	};
	enum ControlType:char
	{
		sampleInterval,//����Ƶ��
		threshold,//�쳣�����ֵ
		recoverTime//�쳣�ָ�ʱ��
	};
	bool static isConnectted;
	
public:
	RMS_Socket();
	//��ʼ����socketͨ��
	void Start();
	static void sendFile(const SOCKET &sockConn, IplImage* frame);
	static void sendFileWithoutDetect(const SOCKET &sockConn, IplImage* frame);
	virtual ~RMS_Socket();
	//���ڹ���ͨ��
	static DWORD WINAPI CommManager(LPVOID lpParam);
	static DWORD GetThreadID();
	//�����ص���Ϣ���У���Ҫ��processor��������Ϣ
	static void ProcessMessage(SOCKET sockConn);
	//���ڱ��ص�С��Int����ת��Ϊ���int
	static int EndianConvertLToB(int);
	//���ת��ΪС��
	static int EndianConvertBToL(int);
	//���ڽ����ֻ��˷���������
	static void ReceviceRequest(SOCKET sockConn,int &timerId);
	static DWORD GetMainThreadId(DWORD processId = 0);
	static VOID   CALLBACK  TimerProc(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime);
};

