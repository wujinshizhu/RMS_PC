#pragma once
#include <winsock.h>
#include <iostream>
#pragma comment(lib, "WS2_32")	// ���ӵ�WS2_32.lib
#define PORT 20000
#include "Processor.h"
class RMS_Socket
{
private:
	//���ڼ���winsock��
	WSADATA wsa;

	//���ڹ���socketͨ�ŵ��߳�
	HANDLE sockThreadHnd;
	//�߳�ID
	DWORD sockThreadId;

	enum type:int
	{
		heartbeat,//type=0ʱ����ʾ������Ϣ�����ڼ��Ŀ���ֻ��Ƿ�����
		picture,//type=1ʱ����ʾ����ͼƬ
		video,//type=2ʱ����ʾ������Ƶ����ʱ���Ƿ��������Ƿ�ʵ��
		control,//type=3ʱ����ʾ���ݿ�����Ϣ
		disconnect//type=4ʱ����ʾ�Ͽ�����
	};
	
public:
	RMS_Socket();
	//��ʼ����socketͨ��
	void Start();
	void sendFile(const SOCKET &sockConn);
	virtual ~RMS_Socket();
	//���ڹ���ͨ��
	static DWORD WINAPI CommManager(LPVOID lpParam);
	DWORD GetThreadID();
	//�����ص���Ϣ���У���Ҫ��processor��������Ϣ
	static void PrcessMessage(SOCKET sockConn);
	//���ڱ��ص�С��Int����ת��Ϊ���int
	static int EndianConvertLToB(int);
};

