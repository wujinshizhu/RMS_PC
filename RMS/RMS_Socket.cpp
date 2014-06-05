#include "RMS_Socket.h"


RMS_Socket::RMS_Socket()
{
	if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
	{
		perror("初始化资源失败");
		exit(-1);
	}
	//创建线程
	sockThreadHnd = CreateThread(NULL, 0, CommManager, NULL, 0, &sockThreadId);
	mainThreadId = GetMainThreadId();
}
//静态变量一定要进行定义，之前class中仅仅只做了声明，不然会报无法解析的错
bool RMS_Socket::isConnectted = false;
DWORD RMS_Socket::sockThreadId = 0;
DWORD RMS_Socket::mainThreadId = 0;
void RMS_Socket::Start()
{
	
}
//用于发送图片给手机端
void RMS_Socket::sendFile(const SOCKET &sockConn, IplImage* frame)
{
	char type_signal = picture;
	if (send(sockConn, (char *)&type_signal, sizeof(type_signal), 0) == SOCKET_ERROR)
	{
		//isConnectted = false;
		printf("网络发生异常，请检测网络连接并确保手机端保持socket连接\n");
		return;
	}
	stringstream stream;
	char* picturePath = new char[100];
	stream << PATH << "ToSend.jpg";
	stream >> picturePath;
	char * path = (LPSTR)picturePath;
	cvSaveImage(path, frame);
	//读取刚刚存储的文件
	FILE *fp;
	fp = fopen(path, "rb");
	fseek(fp, 0, SEEK_END);
	int file_size;
	file_size = ftell(fp);
	char *tmp;
	fseek(fp, 0, SEEK_SET);
	tmp = (char *)malloc(file_size * sizeof(char));
	fread(tmp, sizeof(char), file_size, fp);
	int size = file_size;
	size = EndianConvertLToB(size);
	char * sendBuf = (char*)&size;
	if (send(sockConn, sendBuf, sizeof(sendBuf), 0) == SOCKET_ERROR)
	{
		printf("网络发生异常，请检测网络连接并确保手机端保持socket连接\n");
		//isConnectted = false;
		delete[] picturePath;
		fclose(fp);
		free(tmp);
		if (tmp != NULL)
		{
			tmp = NULL;
		}
		return;
	}
	//send data
	if (send(sockConn, tmp, file_size, 0) == SOCKET_ERROR)
	{
		printf("网络发生异常，请检测网络连接并确保手机端保持socket连接\n");
		//isConnectted = false;
		delete[] picturePath;
		fclose(fp);
		free(tmp);
		if (tmp != NULL)
		{
			tmp = NULL;
		}
		return;
	}
	delete[] picturePath;
	fclose(fp);
	free(tmp);
	if (tmp != NULL)
	{
		tmp = NULL;
	}
}

//没有sock错误检测的的文件发送
void RMS_Socket::sendFileWithoutDetect(const SOCKET &sockConn, IplImage* frame)
{
	char type_signal = picture;
	send(sockConn, (char *)&type_signal, sizeof(type_signal), 0);
	stringstream stream;
	char* picturePath = new char[100];
	stream << PATH << "ToSend.jpg";
	stream >> picturePath;
	char * path = (LPSTR)picturePath;
	cvSaveImage(path, frame);
	//读取刚刚存储的文件
	FILE *fp;
	fp = fopen(path, "rb");
	fseek(fp, 0, SEEK_END);
	int file_size;
	file_size = ftell(fp);
	char *tmp;
	fseek(fp, 0, SEEK_SET);
	tmp = (char *)malloc(file_size * sizeof(char));
	fread(tmp, sizeof(char), file_size, fp);
	int size = file_size;
	size = EndianConvertLToB(size);
	char * sendBuf = (char*)&size;
	send(sockConn, sendBuf, sizeof(sendBuf), 0);
	//send data
	send(sockConn, tmp, file_size, 0);
	delete[] picturePath;
	fclose(fp);
	free(tmp);
	if (tmp != NULL)
	{
		tmp = NULL;
	}
}
//
DWORD WINAPI RMS_Socket::CommManager(LPVOID lpParam)
{
	//用于通信的socket
	SOCKET	mSocket;
	//socket相关地址、协议设定
	SOCKADDR_IN addrSrv;
	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//mSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (mSocket == -1)
	{
		perror("创建socket失败");
		exit(-1);
	}
	//绑定0.0.0.0IP，保证可以监听到所有连接到本端口的连接
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	//转换为网络字节序
	addrSrv.sin_port = htons(PORT);

	bind(mSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	listen(mSocket, 5);
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	
	while (1)
	{
		printf("等待手机端的连接\n");
		SOCKET sockConn = accept(mSocket, (SOCKADDR*)&addrClient, &len);
		printf("检测到socket连接\n");
		u_long iMode = 1;  //non-blocking mode is enabled.
		ioctlsocket(sockConn, FIONBIO, &iMode); //设置为非阻塞模式
		int timerId=SetTimer(NULL, 2, TIME_OVER, TimerProc);
		//用于判断当前是否依然处于连接状态
		isConnectted = true;
		try
		{
			while (isConnectted)
			{
				//接受processor传来的图片并进行存储发送
				ProcessMessage(sockConn);
				//视频发送，待完成
				//断开连接发送，待完成
				if (isConnectted)
				{
					ReceviceRequest(sockConn, timerId);
				}
			}

		}
		catch (Exception * e)
		{
			cout << "通信中出现异常，通信中断，请尝试重新进行连接" << endl;
		}
		
		KillTimer(NULL, timerId);
		closesocket(sockConn);
		printf("断开连接\n");
	}
	closesocket(mSocket);
}

DWORD RMS_Socket::GetThreadID()
{
	return sockThreadId;
}

void RMS_Socket::ProcessMessage(SOCKET sockConn)
{

	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		//将获取的事件分发到对应的回调函数中，在本代码中，该函数用于将超时信息传递给timer的回调函数，从而保证超时判断的有效性
		DispatchMessage(&msg);	
		if (msg.message == UM_WORK)
		{	//当连接断开后不发送图片，转为存在本地文件
			if (isConnectted)
			{
				IplImage* frame = (IplImage*)msg.wParam;
				sendFileWithoutDetect(sockConn, frame);
			}
			else
			{
				//图片存到本地
			}
		}
	}
}

void RMS_Socket::ReceviceRequest(SOCKET sockConn,int &timerId)
{
	char recvType[1];
	if (recv(sockConn, recvType, 1, 0) > 0)
	{

		char recvBuffer[4];
		switch (recvType[0])
		{
		case heartbeat:
			//重置超时计时
			KillTimer(NULL, timerId);
			timerId=SetTimer(NULL, 2, TIME_OVER, TimerProc);
			break;
		case picture:
			break;
		case video:
			break;
		//接收到控制信息，发送信息给主线程，主线程重新启动所有服务（待定）
		case control:
			//接收controlType
			if (recv(sockConn, recvType, 1, 0) > 0)
			{
			
				switch (recvType[0])
				{
				case sampleInterval:
					//发送消息给主线程，主线程重置计时器

					if (recv(sockConn, recvBuffer, sizeof(int), 0) > 0)
					{
						int sample_interval;
						/*memcpy(&sample_interval, recvBuffer, sizeof(int));
						//网络字节序转换为主机字节序
						sample_interval = ntohs(sampleInterval);*/
						sample_interval = (int)recvBuffer[3];
						PostThreadMessageA(mainThreadId, UM_CONTORL_SAMPLEINTERVAL, (WPARAM)sample_interval, NULL);
					}
					break;
				case threshold:
					if (recv(sockConn, recvBuffer, sizeof(int), 0) > 0)
					{
						int threshold = (int)recvBuffer[3];
						PostThreadMessageA(mainThreadId, UM_CONTORL_THRESHOLD, (WPARAM)threshold, NULL);
					}
					break;
				case recoverTime://异常恢复时间，暂时未加入该功能
					
					if (recv(sockConn, recvBuffer, sizeof(int), 0) > 0)
					{
						int recoverTime= (int)recvBuffer[3];
						PostThreadMessageA(mainThreadId, UM_CONTORL_RECOVERTIME, (WPARAM)recoverTime, NULL);
					}
					break;
				default:
					break;
				}
			}
			
			break;
		case disconnect:
			isConnectted = false;
			break;
		case trust:
			PostThreadMessageA(mainThreadId, UM_TRUST, NULL, NULL);
			break;
		default:
			break;
		}
	}
}
//处理手机端心跳超时的问题
VOID   CALLBACK  RMS_Socket::TimerProc(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime)
{
	isConnectted = false;
	cout << "连接超时" << endl;
}
int RMS_Socket::EndianConvertLToB(int InputNum) {
	CHAR *p = (CHAR*)&InputNum;
	return(((INT)*p << 24) + ((INT)*(p + 1) << 16) +
		((INT)*(p + 2) << 8) + (INT)*(p + 3));
}
//大端转小端，有问题
int RMS_Socket::EndianConvertBToL(int InputNum) {
	CHAR *p = (CHAR*)&InputNum;
	return(((INT)*p) + ((INT)*(p + 1) << 8) +
		((INT)*(p + 2) << 16) + (INT)*(p + 3) << 24);
}

RMS_Socket::~RMS_Socket()
{
	WSACleanup();
}

DWORD RMS_Socket::GetMainThreadId(DWORD processId)
{
	if (processId == 0)
		processId = GetCurrentProcessId();

	DWORD threadId = 0;
	THREADENTRY32 te32 = { sizeof(te32) };
	HANDLE threadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (Thread32First(threadSnap, &te32))
	{
		do
		{
			if (processId == te32.th32OwnerProcessID)
			{
				threadId = te32.th32ThreadID;
				break;
			}
		} while (Thread32Next(threadSnap, &te32));
	}
	return threadId;
}