#include "NPTimer.h"
#include"DxLib.h"

NPTimer::NPTimer(Config* config)
{
	NPTimer::config = config;
}

/**
* GetNowHiPerformanceCountをミリ秒で返す関数
*/
double NPTimer::getNowCount()
{

	if (config->UseHiPerformanceTimer == true) {
		return 0.001 * GetNowHiPerformanceCount();
	}
	else {
		return GetNowCount();
	}
}
