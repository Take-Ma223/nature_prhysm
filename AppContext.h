#pragma once
#include"DxLib.h"
#include "STRUCT_OP.h"
#include "STRUCT_CONFIG.h"
#include <functional>
#include "ImageHandle.h"
#include "NPTimer.h"

class Activity;//�v���g�^�C�v�錾

/**
* �A�v���̃R���e�L�X�g�N���X
* ��Ɉ�������݂��܂�
*/
class AppContext {
private:
	std::function<void(Activity*)> startActivityHandler;

public:
	//ActivityController* activityController;

	void startActivity(Activity* activity) {
		startActivityHandler(activity);
	}

	Option* option;//�Q�[�����̐ݒ�
	Config* config;//�Q�[���S�̂̐ݒ�
	double time = 0;//�Q�[������
	NPTimer timer;
	ImageHandle baseHandle = ImageHandle(DX_SCREEN_BACK, Size(1280, 720));

	AppContext(
		std::function<void(Activity*)> handler = NULL,
		Option* option = NULL,
		Config* config = NULL) {

		startActivityHandler = handler;

		AppContext::option = option;
		AppContext::config = config;
		AppContext::time = time;
		AppContext::timer = NPTimer(config);

	};

	void updateTime() {
		time = timer.getNowCount();
	}
};

