#include "Controller.h"
#include <DxLib.h>

void VirtualKeyInput::setIsPressCondition(function<bool(void)> condition)
{
	isPress = condition;
}

void VirtualKeyInput::setOnTapHandler(function<void(void)> handler)
{
	onTapHandler = handler;
}

void VirtualKeyInput::setOnReleaseHandler(function<void(void)> handler)
{
	onReleaseHandler = handler;
}

void VirtualKeyInput::update()
{
	isJustReleased = false;
	isJustPressed = false;

	if (isPress()) {
		if (keyState == Release) {//�������u��
			isJustPressed = true;
			onTapHandler();
		}
		else if (keyState == Press) {//�����ꂽ�u��
		}
		keyState = Press;

	}
	else {

		if (keyState == Release) {//���������Ă�
		}
		else if (keyState == Press) {//�����ꂽ�u��
			isJustReleased = true;
			onReleaseHandler();
		}
		keyState = Release;

	}
}

void Controller::updateInput()
{
	GetHitKeyStateAll(keyInputBuffer);//keyInputBuffer�ɃL�[�̉�����Ԃ�����(�����Ă�����S��0)

	for (int i = 0; i <= 255; i++) {
		if (keyInputBuffer[i] == 0) {
			keyInput[i].keyState = Release;
		}
		else {
			keyInput[i].keyState = Press;
		}
	}

	updateVirtualInput();
}

void Controller::updateVirtualInput()
{
	for (auto virtualKey : virtualKeyInputs) {
		virtualKey->process();
	}
}
