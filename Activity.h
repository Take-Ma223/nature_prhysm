#pragma once
#include "STRUCT_OP.h"
#include "View.h"
#include "NPTimer.h"
#include "Processable.h"
#include "ActivityContext.h"
#include "Log.h"

class AppContext;//プロトタイプ宣言

/// <summary>
/// アクティビティの状態
/// </summary>
enum class ActivityState
{
	Waiting, //待機中
	Running, //実行中
	Pausing, //停止中
	Finished //終了済み
};


/// <summary>
/// アクティビティクラス
/// 継承して各画面を作ります
/// </summary>
class Activity : public Processable
{
public:
	ActivityState activityState = ActivityState::Waiting;

	virtual ~Activity() {};
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

	/// <summary>
	/// 画面を開始するときに親画面から呼ぶ
	/// </summary>
	void start() {
		initLogic();
		initViewHandlers();
		initButton();

		initTimer();
		onStart();
	};

	// Processable を介して継承されました
	virtual void process() override {
		timerProcess();
		buttonProcess();
		logic();
		draw();
	}

	/// <summary>
	/// 他のアクティビティに遷移
	/// </summary>
	/// <param name="activity">遷移先アクティビティ</param>
	void startActivity(Activity* activity) {
		appContext->startActivity(activity);
	};

	/// <summary>
	/// このアクティビティの終了
	/// </summary>
	void finish() {
		activityState = ActivityState::Finished;
	};

protected:
	AppContext* appContext;
	ActivityContext activityContext;
	double GAME_start_time = 0;
	double GAME_passed_time = 0;//単位はms 経過時間

	double LOOP_passed_time = 1;//1ループにかかった時間(us)
	double time_cash = 0;//LOOP_passed_timeを算出するための記憶変数


	virtual void initLogic() {};//ロジック部分の処理
	virtual void initViewHandlers() {};//各Viewのハンドラの設定

	/// <summary>
	/// タイマーの初期化
	/// </summary>
	virtual void initTimer() {
		GAME_start_time = activityContext.getTime();
	};
	virtual void initButton();//ボタン処理の初期化

	/// <summary>
	/// タイマーの計算処理
	/// </summary>
	virtual void timerProcess() {
		GAME_passed_time = activityContext.getTime() - GAME_start_time;//経過時間計算
		LOOP_passed_time = GAME_passed_time - time_cash;//1ループにかかった時間を算出
		time_cash = GAME_passed_time;
	};

	virtual void buttonProcess();//ボタンの処理
	virtual void logic();//毎フレームのロジック処理
	virtual void draw();//描画順番を指定

private:
	Asset asset = Asset();
};

