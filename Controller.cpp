#include "Controller.h"
#include <DxLib.h>

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
		if (keyState == Release) {//‰Ÿ‚µ‚½uŠÔ
			isJustPressed = true;
			onTap();
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
			onRelease();
		}
		keyState = Release;

	}
}

void VirtualKeyInput::process()
{
	update();
}
