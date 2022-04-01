#include"DxLib.h"
#include<sys/stat.h>
#include <future>
#include <thread>

#ifndef _GET_VALUE_FROM_CONTROLLER
#define _GET_VALUE_FROM_CONTROLLER

class GetValueFromController {
private:
	int val = 0;//�R���g���[������󂯎�����l

	int volume = 0;

	BOOL finishFlag = 0;
	HANDLE hComm;
	std::future<int> f1;
	BOOL hasValue = FALSE;//��x�ł��l����M�������ǂ���

	int easing(int input);//3~253��0~1000�ɕϊ�()
public:
	GetValueFromController(HANDLE rcvHComm);
	int getValFromController(int);

	void sendVolumeRequest();

	void start();

	void finish();

	int updateVal(int *reault);
	void getValIfExist(int *value);

};

#endif