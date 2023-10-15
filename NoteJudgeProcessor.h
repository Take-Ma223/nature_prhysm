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
	/// 指定したボタンの色が有効な色の音符かどうか
	/// </summary>
	/// <param name="buttonColor">ボタンの色</param>
	/// <param name="noteColor">音符の色</param>
	/// <returns>true:有効 false:有効ではない</returns>
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
	/// 指定したボタンの色で消せる色の音符かどうか
	/// </summary>
	/// <param name="buttonColor">ボタンの色</param>
	/// <param name="noteColor">音符の色</param>
	/// <returns>true:消せる false:消せない</returns>
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
		assert(false,"noteColorがCMYWの有効色ではありませんでした");
		return NoteColor::F;
	}

public:
	//黒以外のノートのヒット判定
	void NoteJudgeProcessor::GAME_judge_note(int note_search_init, NOTE** note, int lane, int GAME_passed_time, int judge_time_bad, NoteJudgeButtonColor buttonColor, int* timingDifference, int* LN_flag, int* searching) {

		int note_search = note_search_init;

		while (note[lane][note_search].hit == 1 || note[lane][note_search].color_init == NoteColor::K) {
			note_search++;
		}
		*searching = note_search;

		while (1) {
			NoteColor noteColor = (NoteColor)note[lane][note_search].color;

			if (note[lane][note_search].timing_real - GAME_passed_time > judge_time_bad) {//判定対象のノートがまだ判定時間に達していない
				*timingDifference = -10000;
				break;//while(1)を抜ける
			}
			bool isValid = isValidColor(buttonColor, noteColor);
			//通常ノート判定
			if (isValid && note[lane][note_search].group == NoteGroup::Single) {//判定対象のノートが有効色で単ノートのとき

				if (isRemovableColor(buttonColor, noteColor)) {//消せる音符のとき
					note[lane][note_search].hit = 1;
				}
				else {
					note[lane][note_search].color = getHitNoteColorTransition(buttonColor, noteColor);//色を変える
				}

				*timingDifference = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

			//LN始点判定(LNは餡蜜ができないようにする)
			if ((isValid && note[lane][note_search].group == NoteGroup::LongNoteStart)//判定対象のノートにH_TG(またはH_F)が含まれていてLNの始点のとき
				&& ((note_search == note_search_init) || (note[lane][note_search_init].group == NoteGroup::LongNoteEnd && note_search == note_search_init + 1))) {
				//でまだ叩いてないノートがLNの前に無いとき、またはLN終点から探し始めてる時で次のノーツ(LN始点)があるとき

				if (isRemovableColor(buttonColor, noteColor)) {//消せる音符のとき
					LN_flag[lane] = 2;
				}
				else {
					note[lane][note_search].color = getHitNoteColorTransition(buttonColor, noteColor);//色を変える
				}

				*timingDifference = GAME_passed_time - note[lane][note_search].timing_real;
				break;

			}


			if (note[lane][note_search].color == NoteColor::NONE) {//最後以降のノーツに来たら
				*timingDifference = -10000;
				break;
			}
			else {//note_searchのノートがH_TG,H_Fを含んでいない
				do {
					note_search++;//一つ上のノートを探す
					*searching = note_search;
				} while (note[lane][note_search].hit == 1 || note[lane][note_search].color_init == NoteColor::K);//もう叩かれてるノートの番号ならもう一回note_searchをインクリメント
			}
		}

		return;
	}

	//黒ノートのヒット判定
	void NoteJudgeProcessor::GAME_judge_dark_note(int note_search, NOTE** note, int lane, int GAME_passed_time, int judge_time_dark, int* dark_hit, int* searching) {
		int cash = 0;
		while (note[lane][note_search].hit == 1) {//判定枠をまだ通っていない音符までカウンタを進める
			note_search++;
		}
		*searching = note_search;
		while (1) {

			if (note[lane][note_search].timing_real - GAME_passed_time >= judge_time_dark) {//判定対象のノートがまだ判定時間に達していない
				*dark_hit = 0;
				break;//while(1)を抜ける
			}

			cash = note[lane][note_search].color == NoteColor::K;
			if (cash == 1) {//判定対象のノートにH_Kが含まれている

				if (note[lane][note_search].color == NoteColor::K && note[lane][note_search].hit == 0) {//H_Kならhit=1で叩いたことにする
					note[lane][note_search].hit = 1;
					*dark_hit = 1;
					break;
				}

			}

			if (note[lane][note_search].color == NoteColor::NONE) {//最後以降のノーツに来たら
				*dark_hit = 0;
				break;
			}
			else {//note_searchのノートがH_Kを含んでいない
				do {
					note_search++;//一つ上のノートを探す
					*searching = note_search;
				} while (note[lane][note_search].hit == 1);//もう叩かれてるノートの番号ならもう一回note_searchをインクリメント
			}
		}

		return;
	}
};