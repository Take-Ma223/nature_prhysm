#include"GetNowCount_d.h"
#include"DxLib.h"

double GetNowCount_d(Config config) {//GetNowHiPerformanceCountをミリ秒で返す関数
	if (config.UseHiPerformanceTimer == TRUE) {
		return 0.001 * GetNowHiPerformanceCount();
	}
	else {
		return GetNowCount();
	}
}