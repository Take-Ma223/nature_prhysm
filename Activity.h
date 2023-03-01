#pragma once
#include "STRUCT_OP.h"
#include "View.h"
#include "NPTimer.h"
#include "Processable.h"
#include "ActivityContext.h"
#include "Log.h"

class AppContext;//プロトタイプ宣言

enum class ActivityState
{
	Waiting, //待機中
	Running, //実行中
	Pausing, //停止中
	Finished //終了済み
};


/**
* 画面クラス
* 継承して使います
*/
class Activity : public Processable
{
	AppContext* appContext;

	Asset asset = Asset();
protected:
	ActivityContext activityContext;
	double GAME_start_time = 0;
	double GAME_passed_time = 0;//単位はms 経過時間

	double LOOP_passed_time = 1;//1ループにかかった時間(us)
	double time_cash = 0;//LOOP_passed_timeを算出するための記憶変数


	virtual void initLogic() {};//ロジック部分の処理
	virtual void initViewHandlers() {};//各Viewのハンドラの設定
	virtual void initTimer();//タイマーの初期化
	virtual void initButton();//ボタン処理の初期化

	virtual void timerProccess();//タイマーの計算処理
	virtual void buttonProccess();//ボタンの処理
	virtual void logic() {};//毎フレームのロジック処理
	virtual void draw() {};//描画順番を指定
public:
	Activity(AppContext* appContext) {
		Activity::appContext = appContext;
		Activity::activityContext = ActivityContext(appContext, &asset);
	};

	virtual void onCreate() {
		Log(L"onCreate\n");
		ActivityState activityState = ActivityState::Waiting;
	};
	virtual void onStart() {
		Log(L"onStart\n");
		ActivityState activityState = ActivityState::Running;
	};
	virtual void onPause() {
		Log(L"onPause\n");
		ActivityState activityState = ActivityState::Pausing;
	};
	virtual void onDestroy() {
		Log(L"onDestroy\n");
		//asset解放処理
	};

	void start();//画面を開始するときに親画面から呼ぶ

	// Processable を介して継承されました
	virtual void process() override {
		timerProccess();
		buttonProccess();
		logic();
		draw();
	}

	ActivityState activityState = ActivityState::Waiting;

	void startActivity(Activity* activity) {
		appContext->startActivity(activity);
	};
	void finish() {
		activityState = ActivityState::Finished;
	};

};

