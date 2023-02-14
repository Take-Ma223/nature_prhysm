#include "Transition.h"
#include <math.h>


Transition::Transition(double* time) {
	nowTime = time;
}

void Transition::clearEvent() {
	std::vector<Event*> empty;
	
	//メモリ解放
	for (const auto& e : transition) {
		delete[] e;
	}

	std::swap(transition, empty);



	//transition.clear();
	//playStartTime = 0;
}

void Transition::push(Event* input)
{
	transition.push_back(input);
	return;
}

int Transition::getLastIndex(){
	return transition.size() - 1;
}


//プレーヤー関係
void Transition::play()
{
	if (!transition.empty()) {
		if (!isReverse) {
			playStartTime = 0;
			playedTimeOfCaller = getNowTime();
			Transition::isPlay = TRUE;
			durationOffset = 0;
			eventIndex = 0;

			onPlay(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
			//transition[eventIndex].handler();
		}
		else {
			playStartTime = transition.back()->getEndTime();
			playedTimeOfCaller = getNowTime();
			Transition::isPlay = TRUE;
			durationOffset = 0;
			eventIndex = getLastIndex();

			onPlay(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
			//transition[eventIndex].handler();
		}
	}
}

void Transition::stop()
{
	if (Transition::isPlay) {
		Transition::isPlay = FALSE;
	}
}

void Transition::resume()
{
	if (!Transition::isPlay) {
		playStartTime = playingTime;
		playedTimeOfCaller = getNowTime();
		durationOffset = 0;
		Transition::isPlay = TRUE;
	}
}

void Transition::reverse()
{
	if (Transition::isReverse) {
		Transition::isReverse = FALSE;
		if (!transition.empty()) {
			eventIndex = 0;

			onReverse(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
	else {
		Transition::isReverse = TRUE;
		if (!transition.empty()) {
			eventIndex = getLastIndex();

			onReverse(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Transition::setReverse(BOOL isReverse)
{
	Transition::isReverse = isReverse;

	if (!Transition::isReverse) {
		if (!transition.empty()) {
			eventIndex = 0;

			onReverse(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
	else {
		if (!transition.empty()) {
			eventIndex = getLastIndex();

			onReverse(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}

	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Transition::setLoop(BOOL isLoop)
{
	if (!transition.empty()) {
		if (transition.back()->getEndTime() - 0 > 0) {//トランジションに長さがあるときだけ変更許可
			Transition::isLoop = isLoop;
		}
		else {
			Transition::isLoop = FALSE;
		}
	}
}

void Transition::setPlaySpeed(double playSpeed)
{
	Transition::playSpeed = playSpeed;
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Transition::setStartTimeFromTime(int startTime)
{
	Transition::playStartTime = startTime;
	playingTime = playStartTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Transition::setStartTimeFromRange(double startTimeRange)
{
	Transition::playStartTime = startTimeRange * transition.back()->getEndTime();
	playingTime = playStartTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Transition::process()
{
	update();
}


//トランジション処理
void Transition::update()
{
	if (!transition.empty()) {
		if (isPlay) {
			calculateWhereToPlay();
			decideWhichEventToProcess();
			onUpdate();
			//value = calculateVal(transition[eventIndex]);
		}
	}
	return;
}

void Transition::calculateWhereToPlay() {
	//どこを再生するべきか計算
	
	double duration = 0;
	auto calculateDuration = [&] {
		duration = getNowTime() - (playedTimeOfCaller + durationOffset);
		duration *= playSpeed;
	};

	if (!isReverse) {
		//playStartTimeからの相対時間
		calculateDuration();
		playingTime = playStartTime + duration;//playingTimeは必ずトランジションの時間内を指す

		while (playingTime >= transition.back()->getEndTime()) {//トランジションの最後まで到達
			if (isLoop) {//ループ再生時
				durationOffset += transition.back()->getEndTime() / playSpeed;
				calculateDuration();
				eventIndex = 0;

				onLoop(isReverse);
				//transition[eventIndex].determinValueFrom(transition.back().getEndVal(), isReverse);

				playingTime = playStartTime + duration;//playingTimeは必ずトランジションの時間内を指す
			}
			else {//ワンショット時
				//stop();//停止

				//トランジションの最後を指すようにする
				playingTime = transition.back()->getEndTime();
				break;
			}
		}
	}
	else {
		//playStartTimeからの相対時間
		calculateDuration();
		playingTime = playStartTime - duration;//playingTimeは必ずトランジションの時間内を指す

		while (playingTime <= 0) {//トランジションの最初まで到達
			if (isLoop) {//ループ再生時
				durationOffset += transition.back()->getEndTime() / playSpeed;
				calculateDuration();
				eventIndex = getLastIndex();

				onLoop(isReverse);
				//transition[eventIndex].determinValueFrom(transition.front().getStartVal(), isReverse);

				playingTime = playStartTime - duration;//playingTimeは必ずトランジションの時間内を指す
			}
			else {//ワンショット時
				//stop();//停止

				//トランジションの最初を指すようにする
				playingTime = transition.front()->getStartTime();
				break;
			}
		}
	}
}

void Transition::decideWhichEventToProcess()
{
	//処理するべきイベント番号を決める

	if (!isReverse) {
		//今のイベント番号が適切かどうか
		while (1) {
			if ((transition[eventIndex]->getStartTime() <= playingTime) && (playingTime < transition[eventIndex]->getEndTime())
				&& ((transition[eventIndex]->getStartTime()- transition[eventIndex]->getEndTime()) != 0)) {//長さが0ではないイベントの開始終了時間の範囲内



				break;
			}

			eventIndex++;//次のイベントを見る
			if (eventIndex == transition.size()) {//次が無かった  最後まで探索 最後のイベントを指定
				eventIndex--;
				break;
			}

			onThrough(isReverse);
			//transition[eventIndex].determinValueFrom(transition[eventIndex - 1].getEndVal(), isReverse);//このイベントを通るので遷移値を決める

			if (playingTime < transition[eventIndex]->getStartTime()) {//次のイベントの開始時間に達していない　イベントの無い区間にいる場合は直前のイベントを参照
				eventIndex--;
				break;
			}

			onNext(isReverse);
		}
	}
	else {
		//今のイベント番号が適切かどうか
		while (1) {
			if ((transition[eventIndex]->getStartTime() < playingTime) && (playingTime <= transition[eventIndex]->getEndTime())
				&& ((transition[eventIndex]->getStartTime() - transition[eventIndex]->getEndTime()) != 0)) {//長さが0ではないイベントの開始終了時間の範囲内
				break;
			}

			eventIndex--;//次のイベントを見る
			if (eventIndex == -1) {//次が無かった  最後まで探索 最後のイベントを指定
				eventIndex++;
				break;
			}

			onThrough(isReverse);
			//transition[eventIndex].determinValueFrom(transition[eventIndex + 1].getStartVal(), isReverse);//このイベントを通るので遷移値を決める

			if (playingTime > transition[eventIndex]->getEndTime()) {//次のイベントの開始時間に達していない　イベントの無い区間にいる場合は直前のイベントを参照
				eventIndex++;
				break;
			}

			onNext(isReverse);
		}
	}
}



double Transition::getNowTime()
{
	return *nowTime;
}

