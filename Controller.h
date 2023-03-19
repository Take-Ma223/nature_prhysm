#pragma once
#include "DxLib.h"
#include <functional>
#include <vector>
#include "Processable.h"

using namespace std;

enum KeyState
{
	Press,//押している
	Release,//離している
};

class KeyInput
{
public:
	KeyState keyState = Release;
};


/**
* キーコンフィグや複数のキーを一つのキーとして扱うための仮想キー入力クラス
*/
class VirtualKeyInput : public Processable {
public:
	KeyState keyState = Release;
	bool isJustPressed = false;//押した瞬間かどうか
	bool isJustReleased = false;//離した瞬間かどうか

	void setIsPressCondition(function<bool(void)> condition);
	void setOnTapHandler(function<void(void)> handler);
	void setOnReleaseHandler(function<void(void)> handler);

	virtual void process() override {
		update();
	};
private:
	function<bool(void)> isPress;//この仮想キーを押しているとみなす条件
	function<void(void)> onTapHandler = [] {return; };
	function<void(void)> onReleaseHandler = [] {return; };

	void update();
};


class Controller : public Processable
{
public:
	/// <summary>
	/// 継承先で仮想キーを用意し、追加してください
	/// </summary>
	Controller() {};

	virtual void process() override {
		updateInput();
	};

protected:
	KeyInput keyInput[256];

	/// <summary>
	/// 仮想キーの追加
	/// </summary>
	/// <param name="k">仮想キー</param>
	void addVirtualKeyInput(VirtualKeyInput* k) {
		virtualKeyInputs.push_back(k);
	}

private:
	char keyInputBuffer[256];

	void updateInput();//全キーの更新
	void updateVirtualInput();//仮想キーの更新
	std::vector<VirtualKeyInput*> virtualKeyInputs;
};

