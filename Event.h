#pragma once
#include <functional>

class Event {//�l�̕ω�����ێ�����N���X
	double timeStart = 0;
	double timeEnd = 0;
public:
	virtual ~Event() {};
	Event(double timeStart = 0, double  timeEnd = 0);

	double calculateTimeRatio(double now);
	void setTime(int start, int end);

	//getter
	double getStartTime();
	double getEndTime();

};