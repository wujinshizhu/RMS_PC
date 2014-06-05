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
	mainThreadId = GetMainThreadId();
}
//��̬����һ��Ҫ���ж��壬֮ǰclass�н���ֻ������������Ȼ�ᱨ�޷������Ĵ�
bool RMS_Socket::isConnectted = false;
DWORD RMS_Socket::sockThreadId = 0;
DWORD RMS_Socket::mainThreadId = 0;
void RMS_Socket::Start()
{
	
}
//���ڷ���ͼƬ���ֻ���
void RMS_Socket::sendFile(const SOCKET &sockConn, IplImage* frame)
{
	char type_signal = picture;
	if (send(sockConn, (char *)&type_signal, sizeof(type_signal), 0) == SOCKET_ERROR)
	{
		//isConnectted = false;
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
		printf("���緢���쳣�������������Ӳ�ȷ���ֻ��˱���socket����\n");
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

//û��sock������ĵ��ļ�����
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
	//����ͨ�ŵ�socket
	SOCKET	mSocket;
	//socket��ص�ַ��Э���趨
	SOCKADDR_IN addrSrv;
	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//mSocket = socket(AF_INET, SOCK_STREAM, NULL);
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
				if (isConnectted)
				{
					ReceviceRequest(sockConn, timerId);
				}
			}

		}
		catch (Exception * e)
		{
			cout << "ͨ���г����쳣��ͨ���жϣ��볢�����½�������" << endl;
		}
		
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
		{	//�����ӶϿ��󲻷���ͼƬ��תΪ���ڱ����ļ�
			if (isConnectted)
			{
				IplImage* frame = (IplImage*)msg.wParam;
				sendFileWithoutDetect(sockConn, frame);
			}
			else
			{
				//ͼƬ�浽����
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
			//���ó�ʱ��ʱ
			KillTimer(NULL, timerId);
			timerId=SetTimer(NULL, 2, TIME_OVER, TimerProc);
			break;
		case picture:
			break;
		case video:
			break;
		//���յ�������Ϣ��������Ϣ�����̣߳����߳������������з��񣨴�����
		case control:
			//����controlType
			if (recv(sockConn, recvType, 1, 0) > 0)
			{
			
				switch (recvType[0])
				{
				case sampleInterval:
					//������Ϣ�����̣߳����߳����ü�ʱ��

					if (recv(sockConn, recvBuffer, sizeof(int), 0) > 0)
					{
						int sample_interval;
						/*memcpy(&sample_interval, recvBuffer, sizeof(int));
						//�����ֽ���ת��Ϊ�����ֽ���
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
				case recoverTime://�쳣�ָ�ʱ�䣬��ʱδ����ù���
					
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
//���תС�ˣ�������
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