#pragma once
#include "Controller.h"

/// <summary>
/// テスト用コントローラクラス
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
	/// キータップ時のハンドラをセット
	/// </summary>
	/// <param name="handler">ハンドラ</param>
	void setTestKeyTapHandler(function<void(void)> handler) {
		testKey.setOnTapHandler(handler);
	}

private:
	VirtualKeyInput testKey;
};

