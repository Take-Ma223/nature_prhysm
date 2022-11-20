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
	GAME_passed_time = timer.getNowCount() - GAME_start_time;//�o�ߎ��Ԍv�Z
	LOOP_passed_time = GAME_passed_time - time_cash;//1���[�v�ɂ����������Ԃ��Z�o
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


