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
		if (keyState == Release) {//‰Ÿ‚µ‚½uŠÔ
			isJustPressed = true;
			onTapHandler();
		}
		else if (keyState == Press) {//—£‚³‚ê‚½uŠÔ
		}
		keyState = Press;

	}
	else {

		if (keyState == Release) {//—£‚µ‘±‚¯‚Ä‚é
		}
		else if (keyState == Press) {//—£‚³‚ê‚½uŠÔ
			isJustReleased = true;
			onReleaseHandler();
		}
		keyState = Release;

	}
}

void Controller::updateInput()
{
	GetHitKeyStateAll(keyInputBuffer);//keyInputBuffer‚ÉƒL[‚Ì‰Ÿ‰ºó‘Ô‚ª“ü‚é(—£‚µ‚Ä‚¢‚½‚ç‘S‚Ä0)

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
