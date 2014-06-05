#pragma once
#include <fstream>
#include <iostream>
using namespace std;
class Config
{
private:
	//采样间隔 1~5s
	int sampleInterval;
	//相似度阈值50~100
	int threshold;
	//异常恢复时间1~10min
	int recoverTime;
	//文件读入读出流
	ifstream	fin;
	ofstream    fout;
	char * filename;
public:
	Config();
	//对配置内容进行初始化设置
	void Initial();
	//恢复当前配置为默认值
	void RecoverDeault();

	int GetSampleInterval();
	void SetSampleInterval(int sampleInterval);
	int GetThreshold();
	void SetThreshold(int threshold);
	int GetRecoverTime();
	void SetRecoverTime(int recoverTime);
	//保存当前设置到配置文件
	void Save();
	virtual ~Config();
};

