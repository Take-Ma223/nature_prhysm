#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Lane : int {
		NONE,
		MIRROR,
		RANDOM,
		SUPER_RAND
	};
}

class OptionItemLane : public OptionItemBase
{
public:
	OptionItemLane() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"LANE";
		nameList = {
			L"NONE",
			L"MIRROR",
			L"RANDOM",
			L"SUPER RAND"
		};
		descriptionList = {
			L"�I�v�V������t���܂���",
			L"�����̏o�����郌�[�������E���]���܂�",
			L"�����̏o�����郌�[���������_���ɓ���ւ��܂�",
			L"�����̏o������ꏊ�������_���ɕς��܂�"
		};
	}
};