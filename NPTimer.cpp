#include "NPTimer.h"
#include"DxLib.h"

NPTimer::NPTimer(Config* config)
{
	NPTimer::config = config;
}

/**
* GetNowHiPerformanceCount‚ðƒ~ƒŠ•b‚Å•Ô‚·ŠÖ”
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
