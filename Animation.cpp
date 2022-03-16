#include "Animation.h"
#include <math.h>

Converter::Converter(ConvertMode mode, double paramA)
{
	Converter::mode = mode;
	Converter::paramA = paramA;
}

ConvertMode Converter::getMode()
{
	return mode;
}

double Converter::getParamA()
{
	return paramA;
}


point::point(int val, Specify specify) {
	point::val = val;
	point::different = val;
	point::specify = specify;
}

void point::determinValueFrom(int nowVal, BOOL isReverse)
{
	if (specify == Rel) {
		if (isReverse) {
			val = nowVal - different;
		}
		else {
			val = nowVal + different;
		}

	}
}

int point::getVal()
{
	return val;
}


event::event(point start, point end, Converter converter, double timeStart, double  timeEnd) {
	event::start = start;
	event::end = end;

	event::converter = converter;

	event::timeStart = timeStart;
	event::timeEnd = timeEnd;
}

double event::calculateTimeRatio(double now) {
	double ratio = 0;
	if (timeEnd == timeStart) {
		ratio = 1;
	}
	else {
		ratio = (now - timeStart) / (timeEnd - timeStart);
	}
	if (ratio < 0) {
		ratio = 0;
	}
	else if (ratio > 1) {
		ratio = 1;
	}
	return ratio;
}

double event::easing(double input) {
	double output = 0;
	switch (converter.getMode()) {
	case ConvertMode::Teleportation:
		if (input < 1) {
			output = 0;
		}
		else {
			output = input;
		}
		break;
	case ConvertMode::Linear:
		output = input;
		break;
	case ConvertMode::HalfCosine:
		output = (1.0 - cos(input * 3.14159265)) / 2;
		break;
	case ConvertMode::QuarterSine:
		output = sin(input * (3.14159265 / 2));
		break;
	case ConvertMode::QuarterCosine:
		output = 1.0 - cos(input * (3.14159265 / 2));
		break;
	case ConvertMode::Exponential:
		output = 0;
		double startExpY = 1;
		double endExpY = 4;
		double base = converter.getParamA();

		if (base == 1 || base <= 0) {
			output = input;
		}
		else if (base > 1) {
			startExpY = 0;
			endExpY = pow(base, 1) - 1;
			output = (pow(base, input) - 1) / endExpY;
		}
		else if (base < 1) {
			startExpY = 0;
			endExpY = 1 - pow(base, 1);
			output = (1 - pow(base, input)) / endExpY;
		}
		break;
	}
	return output;
}

//setter
void event::setTime(int start, int end) {
	timeStart = start;
	timeEnd = end;
}

void event::determinValueFrom(int nowVal, BOOL isReverse)
{
	start.determinValueFrom(nowVal, isReverse);
	end.determinValueFrom(nowVal, isReverse);
}

//getter
int event::getStartVal() { return start.getVal(); }
int event::getEndVal() { return end.getVal(); }
double event::getStartTime() { return timeStart; }
double event::getEndTime() { return timeEnd; }


//Animationクラス

Animation::Animation(double* time) {
	nowTime = time;
}

int Animation::getValue() {
	return value;
}

void Animation::setValue(int input)
{
	value = input;
	return;
}

//イベント挿入

void Animation::eChange(point start, point end, Converter converter, double durationTime, double startTime) {
	push(
		event(start, end, converter, startTime, startTime + durationTime)
	);
}
void Animation::eChangeTo(point end, Converter converter, double durationTime, double startTime) {
	push(
		event(point(0, Rel), end, converter, startTime, startTime + durationTime)
	);
}
void Animation::eSet(point val, double startTime) {
	push(
		event(val, val, Converter(Teleportation, 1), startTime, startTime)
	);
}
void Animation::eON(double startTime) {
	push(
		event(0, 1, Converter(Teleportation, 1), startTime, startTime)
	);
}
void Animation::eOFF(double startTime) {
	push(
		event(1, 0, Converter(Teleportation, 1), startTime, startTime)
	);
}


void Animation::clearEvent() {
	std::vector<event> empty;
	std::swap(transition, empty);
	//transition.clear();
	playStartTime = 0;
	isLoop = FALSE;
}

void Animation::push(event input)
{
	transition.push_back(input);
	return;
}

int Animation::getLastIndex(){
	return transition.size() - 1;
}


//プレーヤー関係
void Animation::play()
{
	if (!transition.empty()) {
		if (!isReverse) {
			playStartTime = 0;
			playedTimeOfCaller = getNowTime();
			Animation::isPlay = TRUE;
			eventIndex = 0;
			durationOffset = 0;
			transition[eventIndex].determinValueFrom(value, isReverse);
		}
		else {
			playStartTime = transition.back().getEndTime();
			playedTimeOfCaller = getNowTime();
			Animation::isPlay = TRUE;
			eventIndex = getLastIndex();
			durationOffset = 0;
			transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
}

void Animation::stop()
{
	if (Animation::isPlay) {
		Animation::isPlay = FALSE;
	}
}

void Animation::resume()
{
	if (!Animation::isPlay) {
		playStartTime = playingTime;
		playedTimeOfCaller = getNowTime();
		Animation::isPlay = TRUE;
	}
}

void Animation::reverse()
{
	if (Animation::isReverse) {
		Animation::isReverse = FALSE;
	}
	else {
		Animation::isReverse = TRUE;
	}
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
}

void Animation::setReverse(BOOL isReverse)
{
	Animation::isReverse = isReverse;
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
}

void Animation::setLoop(BOOL isLoop)
{
	if (!transition.empty()) {
		if (transition.back().getEndTime() - 0 > 0) {//アニメーションに長さがあるときだけ変更許可
			Animation::isLoop = isLoop;
		}
		else {
			Animation::isLoop = FALSE;
		}
	}
}

void Animation::setPlaySpeed(double playSpeed)
{
	Animation::playSpeed = playSpeed;
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
}


//アニメーション処理
void Animation::update()
{
	if (!transition.empty()) {
		if (isPlay) {
			calculateWhereToPlay();
			decideWhichEventToProcess();
		}
		value = calculateVal(transition[eventIndex]);
	}
	return;
}

void Animation::calculateWhereToPlay() {
	//どこを再生するべきか計算
	
	double duration = 0;
	auto calculateDuration = [&] {
		duration = getNowTime() - (playedTimeOfCaller + durationOffset);
		duration *= playSpeed;
	};

	if (!isReverse) {
		//playStartTimeからの相対時間
		calculateDuration();
		playingTime = playStartTime + duration;//playingTimeは必ずアニメーションの時間内を指す

		while (playingTime > transition.back().getEndTime()) {//アニメーションの最後まで到達
			if (isLoop) {//ループ再生時
				durationOffset += transition.back().getEndTime();
				calculateDuration();
				eventIndex = 0;
				transition[eventIndex].determinValueFrom(transition.back().getEndVal(), isReverse);
				playingTime = playStartTime + duration;//playingTimeは必ずアニメーションの時間内を指す
			}
			else {//ワンショット時
				stop();//停止

				//アニメーションの最後を指すようにする
				playingTime = transition.back().getEndTime();
				break;
			}
		}
	}
	else {
		//playStartTimeからの相対時間
		calculateDuration();
		playingTime = playStartTime - duration;//playingTimeは必ずアニメーションの時間内を指す

		while (playingTime < 0) {//アニメーションの最初まで到達
			if (isLoop) {//ループ再生時
				durationOffset -= transition.back().getEndTime();
				calculateDuration();
				eventIndex = getLastIndex();
				transition[eventIndex].determinValueFrom(transition.front().getStartVal(), isReverse);
				playingTime = playStartTime - duration;//playingTimeは必ずアニメーションの時間内を指す
			}
			else {//ワンショット時
				stop();//停止

				//アニメーションの最初を指すようにする
				playingTime = transition.front().getStartTime();
				break;
			}
		}
	}
}

void Animation::decideWhichEventToProcess()
{
	//処理するべきイベント番号を決める

	if (!isReverse) {
		//今のイベント番号が適切かどうか
		while (1) {
			if ((transition[eventIndex].getStartTime() <= playingTime) && (playingTime <= transition[eventIndex].getEndTime())) {//イベントの開始終了時間の範囲内
				break;
			}

			eventIndex++;//次のイベントを見る
			if (eventIndex == transition.size()) {//次が無かった  最後まで探索 最後のイベントを指定
				eventIndex--;
				break;
			}
			transition[eventIndex].determinValueFrom(transition[eventIndex - 1].getEndVal(), isReverse);

			if (playingTime < transition[eventIndex].getStartTime()) {//次のイベントの開始時間に達していない　イベントの無い区間にいる場合は直前のイベントを参照
				eventIndex--;
				break;
			}
		}
	}
	else {
		//今のイベント番号が適切かどうか
		while (1) {
			if ((transition[eventIndex].getStartTime() <= playingTime) && (playingTime <= transition[eventIndex].getEndTime())) {//イベントの開始終了時間の範囲内
				break;
			}

			eventIndex--;//次のイベントを見る
			if (eventIndex == -1) {//次が無かった  最後まで探索 最後のイベントを指定
				eventIndex++;
				break;
			}
			transition[eventIndex].determinValueFrom(transition[eventIndex + 1].getStartVal(), isReverse);

			if (playingTime > transition[eventIndex].getEndTime()) {//次のイベントの開始時間に達していない　イベントの無い区間にいる場合は直前のイベントを参照
				eventIndex++;
				break;
			}
		}
	}
}

int Animation::calculateVal(event event) {
	//遷移時間経過割合
	double timeRatio = event.calculateTimeRatio(playingTime);

	//遷移量にかける値(この値の変化具合で遷移の仕方が変わる)
	double multipleRatio = event.easing(timeRatio);

	double transitionAmount = (double)event.getEndVal() - event.getStartVal();

	return (int)((double)event.getStartVal() + transitionAmount * multipleRatio);
}

double Animation::getNowTime()
{
	return *nowTime;
}

