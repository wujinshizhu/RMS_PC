#include "Processor.h"
#include <opencv2\features2d\features2d.hpp>



Processor::Processor(int threshold)
{
	this->threshold = threshold;
	processorHnd = CreateThread(NULL, 0, ProcessFrame, NULL, 0, &processorID);
}

int Processor::threshold = 75;
//get the Similarity of the successive frame
DWORD WINAPI Processor::ProcessFrame(LPVOID lpParam)
{
	MSG msg;
	IplImage * frame_last = NULL;
	IplImage * frame_now = NULL;
	DWORD SockThreadID = NULL;
	while (1)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == UM_WORK)
			{
				//double t = getTickCount();
				IplImage* frame = (IplImage*)msg.wParam;
				if (frame_now != NULL)
				{
					cvReleaseImage(&frame_last);
					frame_last = frame_now;
					//copy the image
					IplImage* tempImg = cvCreateImage(cvGetSize(frame),
						frame->depth,
						frame->nChannels);
					cvCopy(frame, tempImg, NULL);
					frame_now = tempImg;
					if (CompareImage(frame_last, frame_now) < threshold)
					{
						sendToSocket(frame_now, SockThreadID);
					}
					//SavePicture(frame_now);
				}
				else
				{
					//copy the image
					IplImage* tempImg = cvCreateImage(cvGetSize(frame),
							frame->depth,
							frame->nChannels);
					cvCopy(frame, tempImg, NULL);
					frame_now = tempImg;
				}
				//t = getTickCount() - t;
				//t = t * 1000 / getTickFrequency();
				//printf("%f ms\n", t);
			}
			else if (msg.message == UM_ID)
			{
				SockThreadID = msg.wParam;
			}
		}
		Sleep(500);
	}

	return 0;
}
//进行图像比较，并返回最小相似度
float Processor::CompareImage(IplImage *image1, IplImage *image2)
{
	Mat i1(image1);
	Mat i2(image2);
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);
	Mat I2_2 = I2.mul(I2);
	Mat I1_2 = I1.mul(I1);
	Mat I1_I2 = I1.mul(I2);
	Mat mu1, mu2;
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	Mat sigma1_2, sigma2_2, sigma12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;
	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;
	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);
	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);
	Mat ssim_map;
	divide(t3, t1, ssim_map);
	Scalar mssim = mean(ssim_map);
	/*
	printf("图像相似度：\n");
	printf("%f\n", mssim.val[0] * 100);
	printf("%f\n", mssim.val[1] * 100);
	printf("%f\n", mssim.val[2] * 100);
	*/
	//返回三者的最小值用作阈值判断
	float min;
	if (mssim.val[0] > mssim.val[1])
	{
		min = mssim.val[1];
	}
	else
	{
		min = mssim.val[0];
	}
	if (min > mssim.val[2])
	{
		printf("图像相似度：%f\n",mssim.val[2]);
		return mssim.val[2]*100;
	}
	else
	{
		printf("图像相似度：%f\n", min);
		return min*100;
	}
}

void Processor::SavePicture(IplImage *image)
{
	static int pictureID = 0;
	char* pictureID_str = new char[100];
	stringstream stream;
	stream <<PATH<<"RMS_"<<pictureID++<<".jpg";
	stream >>pictureID_str;
	char * path = (LPSTR)pictureID_str;
	cvSaveImage(path, image);
}

void Processor::sendToSocket(IplImage * image,DWORD sockThreadID)
{
	PostThreadMessage(sockThreadID, UM_WORK,(WPARAM)image, NULL);
}

DWORD Processor::GetProcessorId()
{
	return processorID;
}

Processor::~Processor()
{
	CloseHandle(processorHnd);
}
