#pragma once

/// <summary>
/// 画面クラス
/// </summary>
class Screen
{
public:
	Screen() {};

	/// <summary>
	/// 画面の開始
	/// </summary>
	void run() {
		init();
		while (isContinueScreen) {
			loop();
		}
	}

protected:
	/// <summary>
	/// 画面の終了
	/// </summary>
	void exitScreen() {
		isContinueScreen = false;
	}

private:
	bool isContinueScreen = true;

	/// <summary>
	/// 画面開始時の初期化処理
	/// </summary>
	virtual void init() = 0;

	/// <summary>
	/// Model更新処理
	/// </summary>
	virtual void updateModel() = 0;

	/// <summary>
	/// UI表示処理
	/// </summary>
	virtual void updateView() = 0;

	/// <summary>
	/// ゲームループ
	/// </summary>
	void loop() {
		updateModel();
		if (!isContinueScreen)return;
		updateView();
		if (!isContinueScreen)return;
	}
};



