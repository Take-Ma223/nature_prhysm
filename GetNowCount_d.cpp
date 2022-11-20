#include"GetNowCount_d.h"
#include"DxLib.h"

double GetNowCount_d(Config config) {//GetNowHiPerformanceCount‚ğƒ~ƒŠ•b‚Å•Ô‚·ŠÖ”
	if (config.UseHiPerformanceTimer == TRUE) {
		return 0.001 * GetNowHiPerformanceCount();
	}
	else {
		return GetNowCount();
	}
}