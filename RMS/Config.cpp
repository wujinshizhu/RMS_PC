#include "Config.h"

//�����ļ���صĲ�����
Config::Config()
{
	filename = "RMS.config";
	fin.open(filename);
	if (!fin)//�ļ��޷��򿪣��ж�Ϊ�ļ������ڣ������ļ���д��Ĭ��ֵ
	{
		fin.close();
		fout.open(filename);
		if (!fout)
		{
			cout << "���������ļ�ʧ��" << endl;
		}
		else
		{
			Initial();
			fout << sampleInterval <<" "<< threshold <<" "<< recoverTime;
			fout.close();
		}
	}
	else
	{
		//��ȡ�����ļ���Ϣ
		fin >> sampleInterval >> threshold >> recoverTime;
		fin.close();
	}
}

void Config::Initial()
{
	//Ĭ�ϵĲ������1s����ֵ75%���ָ�ʱ��1min
	this->sampleInterval = 1;
	this->threshold = 75;
	this->recoverTime = 1;
}

void Config::RecoverDeault()
{
	Initial();
	fout.open(filename);
	fout << sampleInterval << " " << threshold << " " << recoverTime;
	fout.close();
}

int Config::GetSampleInterval()
{
	return sampleInterval;
}

void Config::SetSampleInterval(int sampleInterval)
{
	this->sampleInterval = sampleInterval;
}

int Config::GetThreshold()
{
	return threshold;
}
void Config::SetThreshold(int threshold)
{
	this->threshold = threshold;
}

int Config::GetRecoverTime()
{
	return recoverTime;
}

void Config::SetRecoverTime(int recoverTime)
{
	this->recoverTime = recoverTime;
}
Config::~Config()
{

}
