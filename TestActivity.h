#pragma once
#include "Activity.h"
#include "Log.h"
#include "TestController.h"

/// <summary>
/// �t���[�����[�N�̃e�X�g�p�A�N�e�B�r�e�B
/// </summary>
class TestActivity : public Activity
{
public:
	TestController controller;

	int a = 0;
	TestActivity(AppContext* ac) : Activity(ac) {

	}
	
	TestActivity(AppContext* ac, int input) : Activity(ac) {
		a = input;
	}

	virtual void onCreate() override {
		Activity::onCreate();

		controller.setTestKeyTapHandler([] {
				Log(L"�L�[���^�b�v���܂���");
				printfDx(L"�L�[���^�b�v���܂���");
				return;
			}
		);
	}

	virtual void onStart() override {
		Activity::onStart();
		//
		//Log(to_wstring(a));

		//if (a <= 99999) {
		//	auto act = new TestActivity(appContext, a + 1);
		//	activityContext.startActivity((Activity*)act);
		//}
		//finish();

	};
	
	virtual void buttonProcess() override {
		controller.process();
	}

	virtual void logic() override {
	
	}

	virtual void draw() override {

	}
};

