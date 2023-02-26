#pragma once
#include"DxLib.h"
#include "STRUCT_OP.h"
#include "STRUCT_CONFIG.h"
#include <functional>
#include "ImageHandle.h"

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

