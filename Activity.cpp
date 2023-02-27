#include "Activity.h"
#include "GetNowCount_d.h"
#include "AppContext.h"

void Activity::start()
{
	initLogic();
	initViewHandlers();
	initButton();

	initTimer();
	onStart();
}

void Activity::initTimer()
{
	GAME_start_time = activityContext.getTime();
}

void Activity::timerProccess()
{
	GAME_passed_time = activityContext.getTime() - GAME_start_time;//経過時間計算
	LOOP_passed_time = GAME_passed_time - time_cash;//1ループにかかった時間を算出
	time_cash = GAME_passed_time;
}

void Activity::initButton()
{

}

void Activity::buttonProccess()
{

}



