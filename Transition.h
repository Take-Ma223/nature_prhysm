#pragma once
#include"DxLib.h"
#include <queue>
#include <functional>
#include "Event.h"

class Transition
{
	double* nowTime;//現在のゲーム時間へのポインタ
	
	void calculateWhereToPlay();
	void decideWhichEventToProcess();

	
	double getNowTime();
	int getLastIndex();

protected:
	std::vector<Event*> transition;//イベントの配列

	int value = 0;

	double playSpeed = 1;//再生速度
	double playStartTime = 0;//トランジション内のどの時刻から再生するか
	double playingTime = 0;//トランジション内のどこを再生しているか
	double durationOffset = 0;//playStartTime + durationがトランジションの長さを越えたときにあらかじめトランジション長分引いて範囲内に収まるようにする

	double playedTimeOfCaller = 0;//トランジションを再生(または途中再生)開始したときの呼び出し元の時間
	int eventIndex = 0;//今処理しているイベント番号

	BOOL isPlay = FALSE;//再生しているか

	BOOL isLoop = FALSE;//ループ再生するかどうか
	BOOL isReverse = FALSE;//逆再生するかどうか

	void push(Event*);

	virtual void onPlay(BOOL isReverse) {};
	virtual void onReverse(BOOL isReverse) {};
	virtual void onUpdate() {};
	virtual void onLoop(BOOL isReverse) {};
	virtual void onThrough(BOOL isReverse) {};
	virtual void onNext(BOOL isReverse) {};

public:
	
	Transition(double* time = NULL);
	void update();

	void clearEvent();//transitionを空にして削除

	void play();//最初から再生(逆再生の時は最後から再生)
	void stop();//再生停止
	void resume();//途中から再生
	void reverse();       //現在の再生方向を逆にする
	void setReverse(BOOL);    //どちら向きで再生するか指定
	void setLoop(BOOL);       //ループ再生するかどうか設定
	void setPlaySpeed(double);//再生倍率を設定

	void setStartTimeFromTime(int);//再生位置を設定(時間指定)
	void setStartTimeFromRange(double);//再生位置を設定(割合指定)

};

