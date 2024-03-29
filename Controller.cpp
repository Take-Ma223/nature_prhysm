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
		if (keyState == Release) {//押した瞬間
			isJustPressed = true;
			onTapHandler();
		}
		else if (keyState == Press) {//離された瞬間
		}
		keyState = Press;

	}
	else {

		if (keyState == Release) {//離し続けてる
		}
		else if (keyState == Press) {//離された瞬間
			isJustReleased = true;
			onReleaseHandler();
		}
		keyState = Release;

	}
}

void Controller::updateInput()
{
	GetHitKeyStateAll(keyInputBuffer);//keyInputBufferにキーの押下状態が入る(離していたら全て0)

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
