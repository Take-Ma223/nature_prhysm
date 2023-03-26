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

		auto condition1 = [&] {return keyInput[KEY_INPUT_1].keyState == KeyState::Press; };
		key1.setIsPressCondition(condition1);
		auto condition2 = [&] {return keyInput[KEY_INPUT_2].keyState == KeyState::Press; };
		key2.setIsPressCondition(condition2);
		auto condition3 = [&] {return keyInput[KEY_INPUT_3].keyState == KeyState::Press; };
		key3.setIsPressCondition(condition3);
		auto condition4 = [&] {return keyInput[KEY_INPUT_4].keyState == KeyState::Press; };
		key4.setIsPressCondition(condition4);
		auto condition5 = [&] {return keyInput[KEY_INPUT_5].keyState == KeyState::Press; };
		key5.setIsPressCondition(condition5);
		auto condition6 = [&] {return keyInput[KEY_INPUT_6].keyState == KeyState::Press; };
		key6.setIsPressCondition(condition6);
		auto condition7 = [&] {return keyInput[KEY_INPUT_7].keyState == KeyState::Press; };
		key7.setIsPressCondition(condition7);



		addVirtualKeyInput(testKey);

		addVirtualKeyInput(key1);
		addVirtualKeyInput(key2);
		addVirtualKeyInput(key3);
		addVirtualKeyInput(key4);
		addVirtualKeyInput(key5);
		addVirtualKeyInput(key6);
		addVirtualKeyInput(key7);

	}

	/// <summary>
	/// キータップ時のハンドラをセット
	/// </summary>
	/// <param name="handler">ハンドラ</param>
	void setTestKeyTapHandler(function<void(void)> handler) {
		testKey.setOnTapHandler(handler);
	}

	void setKey1TapHandler(function<void(void)> handler) {
		key1.setOnTapHandler(handler);
	}

	void setKey2TapHandler(function<void(void)> handler) {
		key2.setOnTapHandler(handler);
	}

	void setKey3TapHandler(function<void(void)> handler) {
		key3.setOnTapHandler(handler);
	}

	void setKey4TapHandler(function<void(void)> handler) {
		key4.setOnTapHandler(handler);
	}

	void setKey5TapHandler(function<void(void)> handler) {
		key5.setOnTapHandler(handler);
	}

	void setKey6TapHandler(function<void(void)> handler) {
		key6.setOnTapHandler(handler);
	}

	void setKey7TapHandler(function<void(void)> handler) {
		key7.setOnTapHandler(handler);
	}

private:
	VirtualKeyInput testKey;

	VirtualKeyInput key1;
	VirtualKeyInput key2;
	VirtualKeyInput key3;
	VirtualKeyInput key4;
	VirtualKeyInput key5;
	VirtualKeyInput key6;
	VirtualKeyInput key7;
};

