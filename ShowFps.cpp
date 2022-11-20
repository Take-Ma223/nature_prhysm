#include"ShowFps.h"
#include"DxLib.h"

void ShowFps(double GAME_passed_time, double LOOP_passed_time, double time_cash, Config config) {
	if (config.ShowFps == TRUE) {
		printfDx(L"GAME_passed_time:%f\n", GAME_passed_time);
		printfDx(L"LOOP_passed_time:%f\n", LOOP_passed_time);
		printfDx(L"FPS:%f\n", (double)1000 / LOOP_passed_time);
		//printfDx(L"time_cash:%f\n", time_cash);
	}
}