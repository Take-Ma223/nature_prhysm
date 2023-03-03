#pragma once
#include"DxLib.h"
#include "STRUCT_OP.h"
#include "STRUCT_CONFIG.h"
#include <functional>
#include "ImageHandle.h"
#include "NPTimer.h"
#include "Intent.h"

class Activity;//プロトタイプ宣言

/**
* アプリのコンテキストクラス
* 常に一つだけ存在します
*/
class AppContext {
private:
	Intent* intent;

public:
	//ActivityController* activityController;

	void startActivity(Activity* activity) {
		intent->setActivity(activity);
	}

	Option* option;//ゲーム内の設定
	Config* config;//ゲーム全体の設定
	double time = 0;//ゲーム時間
	NPTimer timer;
	ImageHandle baseHandle = ImageHandle(DX_SCREEN_BACK, Size(1280, 720));

	AppContext(Intent* intent = NULL, Option* option = NULL,	Config* config = NULL) {

		AppContext::option = option;
		AppContext::config = config;
		AppContext::time = time;
		AppContext::timer = NPTimer(config);
		AppContext::intent = intent;
	};

	void updateTime() {
		time = timer.getNowCount();
	}
};

