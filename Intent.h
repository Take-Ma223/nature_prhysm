#pragma once
#include<cassert>

class Activity;//�v���g�^�C�v�錾

class Intent {
	Activity* activity = NULL;

public:
	void setActivity(Activity* activity) {
		assert(Intent::activity == NULL);
		Intent::activity = activity;
	}

	bool isExistActivity() {
		return Intent::activity != NULL;
	}

	Activity* getActivity() {
		assert(Intent::activity != NULL);

		auto activityIntent = Intent::activity;
		Intent::activity = NULL;
		return activityIntent;
	}


};

