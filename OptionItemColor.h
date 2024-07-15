#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Color : int {
		NONE,
		RGB_CHANGE,
		RGB_RAND,
		RGB_RAND_PLUS,
		SUPER_RAND,
		SUPER_RAND_PLUS,
		RGB_ONLY,
		CMY_ONLY,
		W_ONLY,
		RAINBOW
	};
}

class OptionItemColor : public OptionItemBase
{
public:
	OptionItemColor() :OptionItemBase() {}
	bool isRainbow() { return getIndex() == static_cast<int>(OptionItem::Color::RAINBOW); }

private:
	virtual void initData() override {
		title = L"COLOR";
		nameList = {
			L"NONE",
			L"RGB CHANGE",
			L"RGB RAND",
			L"RGB RAND+",
			L"SUPER RAND",
			L"SUPER RAND+",
			L"RGB ONLY",
			L"CMY ONLY",
			L"W ONLY",
			L"RAINBOW"
		};
		descriptionList = {
			L"�I�v�V������t���܂���",
			L"�����S�̂�RGB�l�������_���ɓ���ւ��܂�",
			L"�������ꂼ���RGB�l�������_���ɓ���ւ��܂�",
			L"�������ꂼ���RGB�l�������_���ɓ���ւ��܂�(�����O�m�[�g�I�_���ω�)",
			L"�����̐F�������_���ɕς��܂�",
			L"�����̐F�������_���ɕς��܂�(�����O�m�[�g�I�_���ω�)",
			L"�����̐F����,��,�݂̂ɂȂ�܂�(�X�R�A�͕ۑ�����܂���)",
			L"�����̐F�����F,��,���F�݂̂ɂȂ�܂�(�X�R�A�͕ۑ�����܂���)",
			L"�����̐F�����݂̂ɂȂ�܂�(�X�R�A�͕ۑ�����܂���)",
			L"�����̐F�����F�݂̂ɂȂ�܂�(�X�R�A�͕ʂɕۑ�����܂�)"
		};
	}
};