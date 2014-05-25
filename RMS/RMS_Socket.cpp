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
//��̬����һ��Ҫ���ж��壬֮ǰclass�н���ֻ������������Ȼ�ᱨ�޷������Ĵ�
bool RMS_Socket::isConnectted = false;

void RMS_Socket::Start()
{
	
}
//���ڷ���ͼƬ���ֻ���
void RMS_Socket::sendFile(const SOCKET &sockConn, IplImage* frame)
{
	char type_signal = picture;
	if (send(sockConn, (char *)&type_signal, sizeof(type_signal), 0) == SOCKET_ERROR)
	{
		isConnectted = false;
		printf("���緢���쳣�������������Ӳ�ȷ���ֻ��˱���socket����\n");
		return;
	}
	stringstream stream;
	char* picturePath = new char[100];
	stream << PATH << "ToSend.jpg";
	stream >> picturePath;
	char * path = (LPSTR)picturePath;
	cvSaveImage(path, frame);
	//��ȡ�ոմ洢���ļ�
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
		printf("���緢���쳣�������������Ӳ�ȷ���ֻ��˱���socket����\n");
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
		printf("���緢���쳣�������������Ӳ�ȷ���ֻ��˱���socket����\n");
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
		printf("�ȴ��ֻ��˵�����\n");
		SOCKET sockConn = accept(mSocket, (SOCKADDR*)&addrClient, &len);
		printf("��⵽socket����\n");
		u_long iMode = 1;  //non-blocking mode is enabled.
		ioctlsocket(sockConn, FIONBIO, &iMode); //����Ϊ������ģʽ
		int timerId=SetTimer(NULL, 2, TIME_OVER, TimerProc);
		//�����жϵ�ǰ�Ƿ���Ȼ��������״̬
		isConnectted = true;
		try
		{
			while (isConnectted)
			{
				//����processor������ͼƬ�����д洢����
				ProcessMessage(sockConn);
				//��Ƶ���ͣ������
				//�Ͽ����ӷ��ͣ������
				ReceviceRequest(sockConn,timerId);
				Sleep(500);
			}
		}
		catch (Exception * e)
		{
			cout << "ͨ���г����쳣��ͨ���жϣ��볢�����½�������" << endl;
		}
		
		
		//��⵽����֮�����ļ�
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
		printf("�Ͽ�����\n");
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
		//����ȡ���¼��ַ�����Ӧ�Ļص������У��ڱ������У��ú������ڽ���ʱ��Ϣ���ݸ�timer�Ļص��������Ӷ���֤��ʱ�жϵ���Ч��
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
			//���ó�ʱ��ʱ
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
//�����ֻ���������ʱ������
VOID   CALLBACK  RMS_Socket::TimerProc(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime)
{
	isConnectted = false;
	cout << "���ӳ�ʱ" << endl;
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
