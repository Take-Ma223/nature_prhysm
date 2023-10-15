#pragma once

#include"STRUCT_Song.h"
#include"STRUCT_NOTE.h"
#include<vector>
#include <cassert>

using namespace std;

enum NoteJudgeButtonColor {
	Red = 0,
	Green = 1,
	Blue = 2
};

class NoteJudgeProcessor {
	/// <summary>
	/// �w�肵���{�^���̐F���L���ȐF�̉������ǂ���
	/// </summary>
	/// <param name="buttonColor">�{�^���̐F</param>
	/// <param name="noteColor">�����̐F</param>
	/// <returns>true:�L�� false:�L���ł͂Ȃ�</returns>
	bool isValidColor(NoteJudgeButtonColor buttonColor, NoteColor noteColor) {
		vector<vector<NoteColor>> validColorlist = { { NoteColor::R, NoteColor::Y, NoteColor::M, NoteColor::W, NoteColor::F},
		 { NoteColor::G, NoteColor::Y, NoteColor::C, NoteColor::W, NoteColor::F},
		 { NoteColor::B, NoteColor::C, NoteColor::M, NoteColor::W, NoteColor::F} };

		auto result = find(validColorlist[buttonColor].begin(), validColorlist[buttonColor].end(), noteColor);
		if (result == validColorlist[buttonColor].end()) {
			return false;
		}
		else {
			return true;
		}
	}

	/// <summary>
	/// �w�肵���{�^���̐F�ŏ�����F�̉������ǂ���
	/// </summary>
	/// <param name="buttonColor">�{�^���̐F</param>
	/// <param name="noteColor">�����̐F</param>
	/// <returns>true:������ false:�����Ȃ�</returns>
	bool isRemovableColor(NoteJudgeButtonColor buttonColor, NoteColor noteColor) {
		if (buttonColor == NoteJudgeButtonColor::Red) {
			if (noteColor == NoteColor::R || noteColor == NoteColor::F)return true;
		}
		else if(buttonColor == NoteJudgeButtonColor::Green){
			if (noteColor == NoteColor::G || noteColor == NoteColor::F)return true;
		}
		else if(buttonColor == NoteJudgeButtonColor::Blue){
			if (noteColor == NoteColor::B || noteColor == NoteColor::F)return true;
		}
		return false;
	}


	NoteColor getHitNoteColorTransition(NoteJudgeButtonColor buttonColor, NoteColor noteColor) {
		if (buttonColor == NoteJudgeButtonColor::Red) {
			if (noteColor == NoteColor::Y)return NoteColor::G;
			if (noteColor == NoteColor::M)return NoteColor::B;
			if (noteColor == NoteColor::W)return NoteColor::C;
		}
		else if (buttonColor == NoteJudgeButtonColor::Green) {
			if (noteColor == NoteColor::Y)return NoteColor::R;
			if (noteColor == NoteColor::C)return NoteColor::B;
			if (noteColor == NoteColor::W)return NoteColor::M;
		}
		else if (buttonColor == NoteJudgeButtonColor::Blue) {
			if (noteColor == NoteColor::C)return NoteColor::G;
			if (noteColor == NoteColor::M)return NoteColor::R;
			if (noteColor == NoteColor::W)return NoteColor::Y;
		}
		assert(false,"noteColor��CMYW�̗L���F�ł͂���܂���ł���");
		return NoteColor::F;
	}

public:
	//���ȊO�̃m�[�g�̃q�b�g����
	void NoteJudgeProcessor::GAME_judge_note(int note_search_init, NOTE** note, int lane, int GAME_passed_time, int judge_time_bad, NoteJudgeButtonColor buttonColor, int* timingDifference, int* LN_flag, int* searching) {

		int note_search = note_search_init;

		while (note[lane][note_search].hit == 1 || note[lane][note_search].color_init == NoteColor::K) {
			note_search++;
		}
		*searching = note_search;

		while (1) {
			NoteColor noteColor = (NoteColor)note[lane][note_search].color;

			if (note[lane][note_search].timing_real - GAME_passed_time > judge_time_bad) {//����Ώۂ̃m�[�g���܂����莞�ԂɒB���Ă��Ȃ�
				*timingDifference = -10000;
				break;//while(1)�𔲂���
			}
			bool isValid = isValidColor(buttonColor, noteColor);
			//�ʏ�m�[�g����
			if (isValid && note[lane][note_search].group == NoteGroup::Single) {//����Ώۂ̃m�[�g���L���F�ŒP�m�[�g�̂Ƃ�

				if (isRemovableColor(buttonColor, noteColor)) {//�����鉹���̂Ƃ�
					note[lane][note_search].hit = 1;
				}
				else {
					note[lane][note_search].color = getHitNoteColorTransition(buttonColor, noteColor);//�F��ς���
				}

				*timingDifference = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

			//LN�n�_����(LN���Q�����ł��Ȃ��悤�ɂ���)
			if ((isValid && note[lane][note_search].group == NoteGroup::LongNoteStart)//����Ώۂ̃m�[�g��H_TG(�܂���H_F)���܂܂�Ă���LN�̎n�_�̂Ƃ�
				&& ((note_search == note_search_init) || (note[lane][note_search_init].group == NoteGroup::LongNoteEnd && note_search == note_search_init + 1))) {
				//�ł܂��@���ĂȂ��m�[�g��LN�̑O�ɖ����Ƃ��A�܂���LN�I�_����T���n�߂Ă鎞�Ŏ��̃m�[�c(LN�n�_)������Ƃ�

				if (isRemovableColor(buttonColor, noteColor)) {//�����鉹���̂Ƃ�
					LN_flag[lane] = 2;
				}
				else {
					note[lane][note_search].color = getHitNoteColorTransition(buttonColor, noteColor);//�F��ς���
				}

				*timingDifference = GAME_passed_time - note[lane][note_search].timing_real;
				break;

			}


			if (note[lane][note_search].color == NoteColor::NONE) {//�Ō�ȍ~�̃m�[�c�ɗ�����
				*timingDifference = -10000;
				break;
			}
			else {//note_search�̃m�[�g��H_TG,H_F���܂�ł��Ȃ�
				do {
					note_search++;//���̃m�[�g��T��
					*searching = note_search;
				} while (note[lane][note_search].hit == 1 || note[lane][note_search].color_init == NoteColor::K);//�����@����Ă�m�[�g�̔ԍ��Ȃ�������note_search���C���N�������g
			}
		}

		return;
	}

	//���m�[�g�̃q�b�g����
	void NoteJudgeProcessor::GAME_judge_dark_note(int note_search, NOTE** note, int lane, int GAME_passed_time, int judge_time_dark, int* dark_hit, int* searching) {
		int cash = 0;
		while (note[lane][note_search].hit == 1) {//����g���܂��ʂ��Ă��Ȃ������܂ŃJ�E���^��i�߂�
			note_search++;
		}
		*searching = note_search;
		while (1) {

			if (note[lane][note_search].timing_real - GAME_passed_time >= judge_time_dark) {//����Ώۂ̃m�[�g���܂����莞�ԂɒB���Ă��Ȃ�
				*dark_hit = 0;
				break;//while(1)�𔲂���
			}

			cash = note[lane][note_search].color == NoteColor::K;
			if (cash == 1) {//����Ώۂ̃m�[�g��H_K���܂܂�Ă���

				if (note[lane][note_search].color == NoteColor::K && note[lane][note_search].hit == 0) {//H_K�Ȃ�hit=1�Œ@�������Ƃɂ���
					note[lane][note_search].hit = 1;
					*dark_hit = 1;
					break;
				}

			}

			if (note[lane][note_search].color == NoteColor::NONE) {//�Ō�ȍ~�̃m�[�c�ɗ�����
				*dark_hit = 0;
				break;
			}
			else {//note_search�̃m�[�g��H_K���܂�ł��Ȃ�
				do {
					note_search++;//���̃m�[�g��T��
					*searching = note_search;
				} while (note[lane][note_search].hit == 1);//�����@����Ă�m�[�g�̔ԍ��Ȃ�������note_search���C���N�������g
			}
		}

		return;
	}
};