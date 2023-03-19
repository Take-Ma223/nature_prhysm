#pragma once
#include "Controller.h"

/// <summary>
/// �e�X�g�p�R���g���[���N���X
/// </summary>
class TestController : public Controller
{
public:
	TestController() : Controller() {
		auto condition = [&] {return keyInput[KEY_INPUT_SPACE].keyState == KeyState::Press; };
		testKey.setIsPressCondition(condition);

		addVirtualKeyInput(&testKey);
	}

	/// <summary>
	/// �L�[�^�b�v���̃n���h�����Z�b�g
	/// </summary>
	/// <param name="handler">�n���h��</param>
	void setTestKeyTapHandler(function<void(void)> handler) {
		testKey.setOnTapHandler(handler);
	}

private:
	VirtualKeyInput testKey;
};

