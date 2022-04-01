#include"DxLib.h"
#include<sys/stat.h>
#include <future>
#include <thread>

#ifndef _GET_VALUE_FROM_CONTROLLER
#define _GET_VALUE_FROM_CONTROLLER

class GetValueFromController {
private:
	int val = 0;//コントローラから受け取った値

	int volume = 0;

	BOOL finishFlag = 0;
	HANDLE hComm;
	std::future<int> f1;
	BOOL hasValue = FALSE;//一度でも値を受信したかどうか

	int easing(int input);//3~253を0~1000に変換()
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