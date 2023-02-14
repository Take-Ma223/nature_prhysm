#include "Controller.h"
#include <DxLib.h>

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

Controller::Controller()
{
}

void Controller::process()
{
	updateInput();
}

KeyInput::KeyInput()
{
}

void VirtualKeyInput::setOnTapHandler(function<void(void)> handler)
{
	onTapHandler = handler;
}

void VirtualKeyInput::setOnReleaseHandler(function<void(void)> handler)
{
	onReleaseHandler = handler;
}

void VirtualKeyInput::onTap()
{
	onTapHandler();
}

void VirtualKeyInput::onRelease()
{
	onReleaseHandler();
}

void VirtualKeyInput::update()
{
	isJustReleased = false;
	isJustPressed = false;

	if (isPress()) {
		if (keyState == Release) {//�������u��
			isJustPressed = true;
			onTap();
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
			onRelease();
		}
		keyState = Release;

	}
}

void VirtualKeyInput::process()
{
	update();
}
