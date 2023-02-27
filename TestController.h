#pragma once
#include "Controller.h"
class TestController : public Controller
{
public:
	VirtualKeyInput testKey;

	virtual void addKey() {
		auto condition = [&] {return keyInput[KEY_INPUT_SPACE].keyState == KeyState::Press; };
		testKey.setIsPressCondition(condition);

		auto tapHandler = [&] {return; };
		testKey.setOnTapHandler(tapHandler);

		addVirtualKeyInput(&testKey);
	}

};

