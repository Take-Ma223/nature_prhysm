#pragma once
#include <functional>

class Event {//値の変化情報を保持するクラス
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