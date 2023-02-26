#pragma once
#include"DxLib.h"
#include "STRUCT_OP.h"
#include "STRUCT_CONFIG.h"
#include <functional>
#include "ImageHandle.h"

class Activity;//プロトタイプ宣言

/**
* アプリのコンテキストクラス
* 常に一つだけ存在します
*/
class AppContext {
private:
	std::function<void(Activity*)> startActivityHandler;

public:
	//ActivityController* activityController;

	void startActivity(Activity* activity) {
		startActivityHandler(activity);
	}


	Option* option;//ゲーム内の設定
	Config* config;//ゲーム全体の設定
	double* time;
	ImageHandle baseHandle = ImageHandle(DX_SCREEN_BACK, Size(1280, 720));

	AppContext(
		std::function<void(Activity*)> handler = NULL,
		Option* option = NULL,
		Config* config = NULL,
		double* time = NULL) {

		startActivityHandler = handler;

		AppContext::option = option;
		AppContext::config = config;
		AppContext::time = time;

	};
};

