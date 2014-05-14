#include "RMS_Socket.h"


RMS_Socket::RMS_Socket()
{
	if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
	{
		perror("��ʼ����Դʧ��");
		exit(-1);
	}
	//�����߳�
	sockThreadHnd = CreateThread(NULL, 0, CommManager, NULL, 0, &sockThreadId);
}

void RMS_Socket::Start()
{
	
}

void RMS_Socket::sendFile(const SOCKET &sockConn)
{
	/*
	int size = Processor::getFrameNow()->imageSize;
	char * sendBuf = new char[size + 10];
	char * data = Processor::getFrameNow()->imageData;
	sprintf(sendBuf, "%d%s", &size, data);
	send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
	*/
	/*
	// ���ͽ�������
	//    int bytesSent;
	char sendbuf[1024];//���ͻ�����
	//DWORD dwFileSize;//�ļ���С
	DWORD        dwRead;
	BOOL         bRet;
	char filename[] = "c://MyMain.java";
	HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//dwFileSize=GetFileSize(hFile,NULL);
	while (true)
	{
		bRet = ReadFile(hFile, sendbuf, 1024, &dwRead, NULL);
		if (bRet == FALSE)
		{
			MessageBox(NULL, "Read Buf ERROR!", "Error", MB_OK);
			break;
		}
		else if (dwRead == 0)
		{
			MessageBox(NULL, "Send file OK!", "OK", MB_OK);
			break;
		}
		else
		{
			send(server, sendbuf, dwRead, 0);
		}
	};
	CloseHandle(hFile);
	*/
}
//
DWORD WINAPI RMS_Socket::CommManager(LPVOID lpParam)
{
	//����ͨ�ŵ�socket
	SOCKET	mSocket;
	//socket��ص�ַ��Э���趨
	SOCKADDR_IN addrSrv;
	mSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mSocket == -1)
	{
		perror("����socketʧ��");
		exit(-1);
	}
	//��0.0.0.0IP����֤���Լ������������ӵ����˿ڵ�����
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	//ת��Ϊ�����ֽ���
	addrSrv.sin_port = htons(PORT);

	bind(mSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	listen(mSocket, 5);
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	while (1)
	{
		SOCKET sockConn = accept(mSocket, (SOCKADDR*)&addrClient, &len);
		printf("��⵽socket����");
		PrcessMessage(sockConn);
		
		//��⵽����֮�����ļ�
		/*
		char sendBuf[50];
		sprintf(sendBuf, "Welcome %s to here!", inet_ntoa(addrClient.sin_addr));
		send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
		char recvBuf[50];
		recv(sockConn, recvBuf, 50, 0);
		printf("%s\n", recvBuf);
		*/
		closesocket(sockConn);
	}
	closesocket(mSocket);
}

DWORD RMS_Socket::GetThreadID()
{
	return sockThreadId;
}

void RMS_Socket::PrcessMessage(SOCKET sockConn)
{

	MSG msg;
	while (1)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == UM_WORK)
			{
				/*
				IplImage* frame = (IplImage*)msg.wParam;
				IplImage* tempImg = cvCreateImage(cvGetSize(frame),
					frame->depth,
					frame->nChannels);
				cvCopy(frame, tempImg, NULL);
				int size = tempImg->imageSize;
				char * sendBuf = (char*)&size;
				*/
				/*
				char * sendBuf = new char[size + 10];

				char * data = tempImg->imageData;
				sprintf(sendBuf, "%d%s", &size, data);
				send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
				char * sendBuf = new char[4];
				int size = 100;
				sprintf(sendBuf, "%s", &size);
				//sprintf(sendBuf, "%d", 100);
				*/
				int sendsize = send(sockConn, "fuck", sizeof("fuck")-1, 0);
				//int sendsize2 = send(sockConn, "a", sizeof("a"), 0);
				//int sendsize3= send(sockConn, "a", sizeof("a"), 0);
				sendsize++;
				sendsize--;
			}
		}
		Sleep(500);
	}
}

RMS_Socket::~RMS_Socket()
{
	WSACleanup();
}
