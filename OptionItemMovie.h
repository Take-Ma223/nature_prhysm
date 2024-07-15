#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Movie : int {
		OFF,
		ON_NORMAL,
		ON_CLEAR,
	};
}

class OptionItemMovie : public OptionItemBase
{
public:
	OptionItemMovie() :OptionItemBase() {}

protected:
	void initData() override {
		title = L"MOVIE";
		nameList = {
			L"OFF", 
			L"ON:NORMAL", 
			L"ON:CLEAR"
		};
		descriptionList = {
			L"������Đ����܂���",
			L"������Đ����܂�",
			L"������Đ�����Ƃ��A���ʈȊO���\���ɂ��ē�������₷�����܂�"
		};
	}
};