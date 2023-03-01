#pragma once
#include"DxLib.h"
#include<stack>
#include "Activity.h"
#include <functional>
#include "AppContext.h"
#include "TestActivity.h"
#include"Log.h"

class ActivityController
{
	AppContext context;

	std::stack<Activity*> activities;

	void mainLoop() {
		while (true) {
			Log(L"ActivityControllerLoop");
			if (ProcessMessage() != 0) {
				DxLib_End();
				exit(0);
				return;
			}

			if (!activities.empty())activities.top()->process();
			updateTime();

			if (activities.top()->activityState == ActivityState::Finished) {
				finish();
			}

		}
	}

	void updateTime() {
		context.updateTime();
	}


public:
	ActivityController(Option* option, Config* config) {
		context = AppContext(
			[&](Activity* a) {startActivity(a);},
			option,
			config
		);
	};

	/*
	* 最初のアクティビティの開始
	*/
	void start() {
		startActivity(firstActivity());
		mainLoop();
	};

	void startActivity(Activity* activity) {
		if(!activities.empty())activities.top()->onPause();
		activities.push(activity);
		activities.top()->onCreate();
		activities.top()->onStart();
	};
	void finish() {

		activities.top()->onPause();
		activities.top()->onDestroy();
		delete (activities.top());

		activities.pop();
		if (!activities.empty())activities.top()->onStart();
	};

	Activity* firstActivity() {
		return new TestActivity(&context);
	}

};
