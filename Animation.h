#pragma once
#include"DxLib.h"
#include <queue>

enum ConvertMode {
	Teleportation,
	Linear,
	HalfCosine,
	QuarterSine,
	QuarterCosine,
	Exponential
};

class Converter {
	ConvertMode mode = Linear;
	double paramA = 0;//modeがExponentialのときの底

public:
	Converter(ConvertMode mode = Linear, double paramA = 0);
	ConvertMode getMode();
	double getParamA();

};



enum Specify {
	Abs,//絶対
	Rel//相対
};

//値の絶対相対指定クラス
class point {
	int val = 0;
	int different = 0;
	Specify specify = Abs;//Relのときはdifferentを相対値として扱う

public:
	point(int val = 0, Specify specify = Abs);

	void determinValueFrom(int nowVal, BOOL isReverse = FALSE);//現在の値に相対値を加える、イベント実行時に実行
	int getVal();
};

class event {//値の変化情報を保持するクラス


	point start;//相対指定のときは、このイベント実行時にstartValの値にその時の自身の値が加算される
	point end;  //相対指定のときは、このイベント実行時にendValの値にその時の自身の値が加算される

	Converter converter;

	double timeStart = 0;
	double timeEnd = 0;

public:
	event(point start = point(), point end = point(), Converter converter = Converter(Linear, 0), double timeStart = 0, double  timeEnd = 0);

	double calculateTimeRatio(double now);
	double easing(double input);

	void setTime(int start, int end);

	void determinValueFrom(int nowVal, BOOL isReverse = FALSE);//現在の値に相対値を加える、イベント実行時に実行

	//getter
	int getStartVal();
	int getEndVal();

	double getStartTime();
	double getEndTime();


};

class eventAbsAbs :public event {





};




class Animation
{
	double* nowTime;//現在のゲーム時間へのポインタ

	std::vector<event> transition;//イベントの配列
	int value = 0;

	double playSpeed = 1;//再生速度

	double playStartTime = 0;//アニメーション内のどの時刻から再生するか
	double playingTime = 0;//アニメーション内のどこを再生しているか
	double durationOffset = 0;//playStartTime + durationがアニメーションの長さを越えたときにあらかじめアニメーション長分引いて範囲内に収まるようにする


	double playedTimeOfCaller = 0;//アニメーションを再生(または途中再生)開始したときの呼び出し元の時間

	int eventIndex = 0;//今処理しているイベント番号

	BOOL isPlay = FALSE;//再生しているか

	BOOL isLoop = FALSE;//ループ再生するかどうか
	BOOL isReverse = FALSE;//逆再生するかどうか
	
	void calculateWhereToPlay();
	void decideWhichEventToProcess();

	int calculateVal(event event);
	double getNowTime();
	void push(event);

	int getLastIndex();
public:
	Animation(double* time = NULL);
	int getValue();
	void setValue(int);//初期化以外では使わない
	void update();


	void eChange(point start, point end, Converter converter = Converter(Teleportation, 4), double durationTime = 0, double startTime = 0);
	void eChangeTo(point end, Converter converter = Converter(Teleportation, 4), double durationTime = 0, double startTime = 0);
	void eSet(point Abs, double startTime = 0);
	void eON(double startTime = 0);
	void eOFF(double startTime = 0);


	void clearEvent();//transitionを空にして削除

	void play();//最初から再生(逆再生の時は最後から再生)
	void stop();//再生停止
	void resume();//途中から再生
	void reverse();       //現在の再生方向を逆にする
	void setReverse(BOOL);    //どちら向きで再生するか指定
	void setLoop(BOOL);       //ループ再生するかどうか設定
	void setPlaySpeed(double);//再生倍率を設定

};

