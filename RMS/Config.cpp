#include "Config.h"

//配置文件相关的操作类
Config::Config()
{
	filename = "RMS.config";
	fin.open(filename);
	if (!fin)//文件无法打开，判断为文件不存在，创建文件并写入默认值
	{
		fin.close();
		fout.open(filename);
		if (!fout)
		{
			cout << "创建配置文件失败" << endl;
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
		//读取配置文件信息
		fin >> sampleInterval >> threshold >> recoverTime;
		fin.close();
	}
}

void Config::Initial()
{
	//默认的采样间隔1s，阈值75%，恢复时间1min
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
