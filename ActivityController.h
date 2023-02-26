#pragma once
#include"DxLib.h"
#include<stack>
#include "Activity.h"
#include <functional>
#include "AppContext.h"

class ActivityController
{
	AppContext context;

	std::stack<Activity*> activities;

	double time = 0;//ƒQ[ƒ€“àŽžŠÔ
	NPTimer timer;

	void mainLoop() {
		while (true) {
			if (ProcessMessage() != 0) {
				DxLib_End();
				exit(0);
				return;
			}

			activities.top()->process();
			updateTime();

			if (activities.top()->activityState == ActivityState::Finished) {
				finish();
			}

		}
	}

	void updateTime() {
		time = timer.getNowCount();
	}


public:
	ActivityController(Option* option, Config* config) {
		context = AppContext(
			[&](Activity* a) {startActivity(a);},
			option,
			config,
			&time
		);
		timer = NPTimer(config);
	};

	void start(Activity* activity) {
		startActivity(activity);
		mainLoop();
	};

	void startActivity(Activity* activity) {
		activities.top()->onPause();
		activities.push(activity);
		activities.top()->onCreate();
		activities.top()->onStart();
	};
	void finish() {

		activities.top()->onPause();
		activities.top()->onDestroy();
		delete (activities.top());

		activities.pop();
		activities.top()->onStart();
	};

};
