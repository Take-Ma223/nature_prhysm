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
	GAME_passed_time = activityContext.getTime() - GAME_start_time;//�o�ߎ��Ԍv�Z
	LOOP_passed_time = GAME_passed_time - time_cash;//1���[�v�ɂ����������Ԃ��Z�o
	time_cash = GAME_passed_time;
}

void Activity::initButton()
{

}

void Activity::buttonProccess()
{

}



