#pragma once
#include <functional>
#include <vector>

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
	KeyInput();
};


struct KeyBehavior
{

};



/**
* キーコンフィグや複数のキーを一つのキーとして扱うための仮想キー入力クラス
*/
class VirtualKeyInput {
	function<void(void)> onTapHandler = [] {};
	function<void(void)> onReleaseHandler = [] {};

public:
	KeyState keyState = Release;
	bool isJustPressed = false;//押した瞬間かどうか
	bool isJustReleased = false;//離した瞬間かどうか

	void setOnTapHandler(function<void(void)> handler);
	void setOnReleaseHandler(function<void(void)> handler);

	void onTap();
	void onRelease();

	void update();

	function<bool(void)> isPress;//この仮想キーを押しているとみなす条件
};


class Controller
{
	char keyInputBuffer[256];
	KeyInput keyInput[256];

	//継承先でVirtualKeyInputを必要なだけ宣言する
public:
	//継承先でVirtualKeyInputのisPressを設定する
	Controller();

	void updateInput();//毎フレーム呼ぶ

	/**
	* 仮想キー入力を制御する処理を継承先でオーバーライドする
	*/
	virtual void updateVirtualInput() = 0;

	//継承先で仮想キーのハンドラセットメソッドを必要なだけ用意する

};

