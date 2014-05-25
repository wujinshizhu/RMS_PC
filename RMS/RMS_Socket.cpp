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
}
//静态变量一定要进行定义，之前class中仅仅只做了声明，不然会报无法解析的错
bool RMS_Socket::isConnectted = false;

void RMS_Socket::Start()
{
	
}
//用于发送图片给手机端
void RMS_Socket::sendFile(const SOCKET &sockConn, IplImage* frame)
{
	char type_signal = picture;
	if (send(sockConn, (char *)&type_signal, sizeof(type_signal), 0) == SOCKET_ERROR)
	{
		isConnectted = false;
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
		isConnectted = false;
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
		isConnectted = false;
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
//
DWORD WINAPI RMS_Socket::CommManager(LPVOID lpParam)
{
	//用于通信的socket
	SOCKET	mSocket;
	//socket相关地址、协议设定
	SOCKADDR_IN addrSrv;
	mSocket = socket(AF_INET, SOCK_STREAM, 0);
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
				ReceviceRequest(sockConn,timerId);
				Sleep(500);
			}
		}
		catch (Exception * e)
		{
			cout << "通信中出现异常，通信中断，请尝试重新进行连接" << endl;
		}
		
		
		//检测到连接之后发送文件
		/*
		char sendBuf[50];
		sprintf(sendBuf, "Welcome %s to here!", inet_ntoa(addrClient.sin_addr));
		send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
		char recvBuf[50];
		recv(sockConn, recvBuf, 50, 0);
		printf("%s\n", recvBuf);
		*/
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
		{
				
				IplImage* frame = (IplImage*)msg.wParam;
				/*
				IplImage* tempImg = cvCreateImage(cvGetSize(frame),
					frame->depth,
					frame->nChannels);
				cvCopy(frame, tempImg, NULL);
				*/
				sendFile(sockConn, frame);
		}
	}
}

void RMS_Socket::ReceviceRequest(SOCKET sockConn,int &timerId)
{
	char recvType[1];
	if (recv(sockConn, recvType, 1, 0) > 0)
	{
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
		case control:
			break;
		case disconnect:
			isConnectted = false;
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
RMS_Socket::~RMS_Socket()
{
	WSACleanup();
}
