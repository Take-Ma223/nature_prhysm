#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Sort : int {
		DEFAULT,
		LEVEL,
		SCORE,
		CLEAR_STATE,
		MIN_MISS,
		PLAY_COUNT,
		RADAR_TOTAL,
		GLOBAL,
		LOCAL,
		CHAIN,
		UNSTABILITY,
		STREAK,
		COLOR,
		RED_DENSITY,
		GREEN_DENSITY,
		BLUE_DENSITY,
		CYAN_DENSITY,
		MAGENTA_DENSITY,
		YELLOW_DENSITY,
		WHITE_DENSITY,
		BLACK_DENSITY,
		RAINBOW_DENSITY,
		MAX_BPM,
		MIN_BPM,
		MAX_CHORDS,
		VERSION,
	};
}

class OptionItemSort : public OptionItemBase
{
public:
	OptionItemSort() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"SORT";
		nameList = {
			L"DEFAULT", 
			L"LEVEL", 
			L"SCORE", 
			L"CLEAR STATE",
			L"MIN MISS", 
			L"PLAY COUNT",
			L"RADAR TOTAL",
			L"GLOBAL",
			L"LOCAL",
			L"CHAIN",
			L"UNSTABILITY",
			L"STREAK",
			L"COLOR",
			L"RED",
			L"GREEN",
			L"BLUE",
			L"CYAN",
			L"MAGENTA",
			L"YELLOW",
			L"WHITE",
			L"BLACK",
			L"RAINBOW",
			L"MAX BPM",
			L"MIN BPM",
			L"MAX CHORDS",
			L"VERSION"
		};
		descriptionList = {
			 L"�f�t�H���g�̕��я��ɂ��܂�",
			 L"��Փx(�~���m��)�ŕ��ёւ��܂�",
			 L"�X�R�A�ŕ��ёւ��܂�",
			 L"�N���A��Ԃŕ��ёւ��܂�",
			 L"�ŏ��~�X���ŕ��ёւ��܂�",
			 L"�v���C�񐔂ŕ��ёւ��܂�",
			 L"���[�_�[�̍��v�l�ŕ��ёւ��܂�",
			 L"���[�_�[��GLOBAL�l�ŕ��ёւ��܂�",
			 L"���[�_�[��LOCAL�l�ŕ��ёւ��܂�",
			 L"���[�_�[��CHAIN�l�ŕ��ёւ��܂�",
			 L"���[�_�[��UNSTABILITY�l�ŕ��ёւ��܂�",
			 L"���[�_�[��STREAK�l�ŕ��ёւ��܂�",
			 L"���[�_�[��COLOR�l�ŕ��ёւ��܂�",
			 L"�ԐF�����̖��x�ŕ��ёւ��܂�",
			 L"�ΐF�����̖��x�ŕ��ёւ��܂�",
			 L"�F�����̖��x�ŕ��ёւ��܂�",
			 L"���F�����̖��x�ŕ��ёւ��܂�",
			 L"���F�����̖��x�ŕ��ёւ��܂�",
			 L"���F�����̖��x�ŕ��ёւ��܂�",
			 L"���F�����̖��x�ŕ��ёւ��܂�",
			 L"���F�����̖��x�ŕ��ёւ��܂�",
			 L"���F�����̖��x�ŕ��ёւ��܂�",
			 L"�ő�BPM�ŕ��ёւ��܂�",
			 L"�ŏ�BPM�ŕ��ёւ��܂�",
			 L"�ő哯���������ŕ��ёւ��܂�",
			 L"���ʂ����^���ꂽ�o�[�W�����ŕ��ёւ��܂�"
		};
	}
};