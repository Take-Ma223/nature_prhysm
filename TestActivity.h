#pragma once
#include "Activity.h"
#include "Log.h"

class TestActivity : public Activity
{
public:
	int a = 0;
	TestActivity(AppContext* ac) : Activity(ac) {

	}
	
	TestActivity(AppContext* ac, int input) : Activity(ac) {
		a = input;
	}

	virtual void onStart() override {
		Activity::onStart();
		
		Log(to_wstring(a));

		if (a <= 99999) {
			auto act = new TestActivity(appContext, a + 1);
			activityContext.startActivity((Activity*)act);
		}
		finish();

	};

	virtual void logic() override {

	}

	virtual void draw() override {

	}
};

