#pragma once
#include <fstream>
#include <iostream>
using namespace std;
class Config
{
private:
	//������� 1~5s
	int sampleInterval;
	//���ƶ���ֵ50~100
	int threshold;
	//�쳣�ָ�ʱ��1~10min
	int recoverTime;
	//�ļ����������
	ifstream	fin;
	ofstream    fout;
	char * filename;
public:
	Config();
	//���������ݽ��г�ʼ������
	void Initial();
	//�ָ���ǰ����ΪĬ��ֵ
	void RecoverDeault();

	int GetSampleInterval();
	void SetSampleInterval(int sampleInterval);
	int GetThreshold();
	void SetThreshold(int threshold);
	int GetRecoverTime();
	void SetRecoverTime(int recoverTime);
	//���浱ǰ���õ������ļ�
	void Save();
	virtual ~Config();
};

