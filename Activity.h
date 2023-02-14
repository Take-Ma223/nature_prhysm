#pragma once
#include "STRUCT_OP.h"
#include "View.h"
#include "NPTimer.h"
#include "Processable.h"

/**
* アクティビティクラスの前提となるコンテキストクラス
*/
class ActivityContext {
public:
	Option* option;
	Config* config;

	ActivityContext(Option* option = NULL, Config* config = NULL);
};


/**
* 画面クラス
* 継承して使います
*/
class Activity : public Processable
{
	DrawableContext dContext;
	NPTimer timer;

protected:
	ActivityContext* ac;
	double GAME_start_time = 0;
	double GAME_passed_time = 0;//単位はms 経過時間

	double LOOP_passed_time = 1;//1ループにかかった時間(ms)
	double time_cash = 0;//LOOP_passed_timeを算出するための記憶変数


	virtual void initLogic() = 0;//ロジック部分の処理
	virtual void initViewHandlers() = 0;//各Viewのハンドラの設定
	virtual void onStart() = 0;//画面開始する瞬間の処理(あまり重い処理は書かない)

	virtual void initTimer();//タイマーの初期化


	virtual void initButton();//ボタン処理の初期化

	void loop();//メインループ
	virtual void timerProccess();//タイマーの計算処理
	virtual void buttonProccess();//ボタンの処理
	virtual void logic() = 0;//毎フレームのロジック処理
	virtual void draw() = 0;//描画順番を指定
public:
	Activity(ActivityContext* ac){
		Activity::ac = ac;
		Activity::dContext = DrawableContext(&Asset(), ac->option, &GAME_passed_time);
		Activity::timer = NPTimer(ac->config);
	}

	void start();//画面を開始するときに親画面から呼ぶ

	// Processable を介して継承されました
	virtual void process() override;

};

