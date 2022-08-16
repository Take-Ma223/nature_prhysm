#pragma once
#include <functional>
#include"Event.h"

class ActionEvent : public Event {
	//���̃C�x���g�����s����Ƃ��Ɉꏏ�Ɏ��s������������ێ�����n���h��
	std::function<void(void)> handler;

public:
	ActionEvent(double timeStart = 0, double  timeEnd = 0, std::function<void(void)> handler = [] {});

	void action();

};