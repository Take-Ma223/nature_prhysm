#pragma once
#include <functional>
#include"Event.h"

class ActionEvent : public Event {
	//このイベントを実行するときに一緒に実行したい処理を保持するハンドラ
	std::function<void(void)> handler;

public:
	ActionEvent(double timeStart = 0, double  timeEnd = 0, std::function<void(void)> handler = [] {});

	void action();

};