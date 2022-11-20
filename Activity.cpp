#include "Activity.h"
#include "GetNowCount_d.h"

ActivityContext::ActivityContext(Option* option, Config* config)
{
	ActivityContext::option = option;
	ActivityContext::config = config;
}


Activity::Activity(ActivityContext* ac)
{
	Activity::ac = ac;
	Activity::vc = ViewContext(&Asset(), ac->option, &GAME_passed_time);
	Activity::timer = NPTimer(ac->config);

}

void Activity::start()
{
	initLogic();
	initViewHandlers();
	initButton();

	initTimer();
	onStart();

	loop();
}

void Activity::initTimer()
{
	GAME_start_time = timer.getNowCount();
}

void Activity::timerProccess()
{
	GAME_passed_time = timer.getNowCount() - GAME_start_time;//経過時間計算
	LOOP_passed_time = GAME_passed_time - time_cash;//1ループにかかった時間を算出
	time_cash = GAME_passed_time;
}

void Activity::initButton()
{

}

void Activity::buttonProccess()
{

}




void Activity::loop()
{
	timerProccess();
	buttonProccess();
	logic();
	draw();
}


