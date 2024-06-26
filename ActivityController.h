#pragma once
#include"DxLib.h"
#include<stack>
#include "Activity.h"
#include <functional>
#include "AppContext.h"
#include "TestActivity.h"
#include"Log.h"
#include "Intent.h"
#include "DxLibUtil.h"
#include "CONSTANT_VALUE.h"

class ActivityController
{
	Intent intent;
	AppContext context;

	std::stack<Activity*> activities;

	void mainLoop() {
		while (true) {
			//Log(L"ActivityControllerLoop");
			updateTime();
			handleProcessMessage();
			checkIntent();
			activityProcess();
			handleFinishing();

		}
	}

	void updateTime() {
		context.updateTime();
	}

	void handleProcessMessage() {
		if (ProcessMessage() != 0) {
			dxLibFinishProcess();
			return;
		}
	}

	void checkIntent() {
		if (intent.isExistActivity()) {
			startActivity(intent.getActivity());
		}
	}

	void activityProcess() {
		SetDrawScreen(context.baseHandle.getHandle());
		ClearDrawScreen();//�O���t�B�b�N��������
		if (!activities.empty())activities.top()->process();
		ScreenFlip();//��ʂ̔��f
		clsDx();
	}

	void handleFinishing() {
		if (!activities.empty()) {
			if (activities.top()->activityState == ActivityState::Finished) {
				finish();
			}
		}
	}

public:
	ActivityController(Option* option, Config* config) {
		context = AppContext(&intent, option, config);
	};

	/*
	* �ŏ��̃A�N�e�B�r�e�B�̊J�n
	*/
	void start() {
		intent.setActivity(firstActivity());
		mainLoop();
	};

	void startActivity(Activity* activity) {
		//Log(L"size:" + to_wstring(activities.size()));
		if(!activities.empty())activities.top()->onPause();
		activities.push(activity);
		activities.top()->onCreate();
		activities.top()->onStart();
	};

	void finish() {
		//Log(L"size:" + to_wstring(activities.size()));

		activities.top()->onPause();
		activities.top()->onDestroy();
		delete activities.top();

		activities.pop();
		if (!activities.empty())activities.top()->onStart();
	};

	Activity* firstActivity() {
		return new FIRST_ACTIVITY;
	}

};
