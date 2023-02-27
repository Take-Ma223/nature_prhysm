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
	function<bool(void)> isPress;//この仮想キーを押しているとみなす条件
	function<void(void)> onTapHandler = [] {};
	function<void(void)> onReleaseHandler = [] {};

	void update();
public:
	KeyState keyState = Release;
	bool isJustPressed = false;//押した瞬間かどうか
	bool isJustReleased = false;//離した瞬間かどうか

	void setIsPressCondition(function<bool(void)> condition);
	void setOnTapHandler(function<void(void)> handler);
	void setOnReleaseHandler(function<void(void)> handler);

	virtual void process() override;

};


class Controller : public Processable
{
	char keyInputBuffer[256];
	
	void updateInput();//全キーの更新
	void updateVirtualInput();//仮想キーの更新
	std::vector<VirtualKeyInput*> virtualKeyInputs;
protected:
	KeyInput keyInput[256];

	/*
	* 仮想キーの追加
	*/
	void addVirtualKeyInput(VirtualKeyInput* k) {
		virtualKeyInputs.push_back(k);
	}

public:
	Controller();

	//継承先で仮想キーを用意し、追加してください
	virtual void addKey() = 0;

	virtual void process() override;
};

