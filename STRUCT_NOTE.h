﻿#include"DxLib.h"
#include<vector>
#include <stdexcept>
#include <functional>

#ifndef _STRUCT_NOTE
#define _STRUCT_NOTE

const int TIMING_SHOW_RATIO = 1000;//表示用タイミングをミリ秒から何倍の精度で扱うか

enum class NoteFall : char {
	NotFaling,//落ちてない
	Faling,//落下中
	Failed//落ち切った
};

enum class NoteColor : char
{
	NONE = 0,//NoteColor::NONE
	R = 1,//NoteColor::R
	G = 2,//NoteColor::G
	B = 3,//NoteColor::B
	C = 4,//NoteColor::C
	M = 5,//NoteColor::M
	Y = 6,//NoteColor::Y
	W = 7,//NoteColor::W
	K = 8,//NoteColor::K
	F = 9//NoteColor::F
};

enum NoteGroup {
	None = -1,//譜面エディタでのみ使用
	Single = 0,//NoteGroup::Single
	LongNoteStart = 1,//NoteGroup::LongNoteStart
	LongNoteMiddle = 3,//NoteGroup::LongNoteMiddle
	LongNoteEnd = 2,//NoteGroup::LongNoteEnd
};


class NOTE {
public:
	float x = 0;//ノートのx座標
	float y = -128;//ノートのy座標
	int timing = 0;//表示用タイミング(マイクロ秒)
	int timing_real = 0;//叩く実際のタイミング(ミリ秒)
	int timing_init = 0;//LN始点の表示用タイミング保存用変数
	int timing_init_real = 0;//LNの実際に叩くタイミング保存用変数
	NoteFall fall = NoteFall::NotFaling;//0なら落ちてない,1なら落下中,2なら落ち切った
	NoteColor color_init = NoteColor::NONE;//ノートの元の色
	NoteColor color = NoteColor::NONE;//ノートの色(1~9の数字)(1:R 2:G 3:B 4:Y 5:C 6:M 7:W 8:K 9:F(FullColor) 10:ロングノート終点)0ならこのノートは使ってない
	float bpm = 1;//このノートのSCROLL抜きの速さ(BPM)
	float bpm_real = 1;//SCROLLの影響を考慮したこのノートの実際の速さ(BPM基準)
	BOOL hit = 0;//1:叩いた 0:叩いてない
	NoteGroup group = NoteGroup::Single;//ノートの種類(0:単ノート 1:ロングノート始点 2:ロングノート終点)
	NoteGroup group_init = NoteGroup::Single;//ノートの元の種類(0:単ノート 1:ロングノート始点 2:ロングノート終点)
	BOOL LN_k = 0;//LNの終端で離すべきか(0:離さなくても良い 1:離さないといけない黒終点)
	short textLine = 0;//譜面データ上の行(見つけやすくする)
	BOOL isBright = 0;//鳴らす音を大きくするかどうか(0:通常 1で長くする) 長く鳴る音符は点滅する
	float brightRingAlpha = 0;//光る音符の輪の透明度(出現時にフェードインさせる)


	BOOL ignore = 0;//ワープ区間のため無視される音符なら1 それ以外0
	//std::vector<GradationLongNoteNode*> j;
};


class NoteSearcher {
	NOTE** note;

public:
	NoteSearcher(NOTE** n) {
		note = n;

	}

	/// <summary>
	/// laneとindexの位置から一番最近のSingleノートのindexを返す
	/// </summary>
	int getLatestSingle(int lane, int ind) {
		int index = ind;

		while (true) {
			if (note[lane][index].group == NoteGroup::Single) {
				return index;
			}
			index--;
			if (index < 0) {
				throw std::runtime_error("単ノートが見つかりませんでした。");
			}
		}
	}

	/// <summary>
	/// laneとindexの位置(ロング中のみ)からロングノート始点のindexを返す
	/// </summary>
	int searchLnStart(int lane, int ind, std::function<void(NOTE&)> handler = nullptr) {
		int index = ind;

		if (note[lane][index].group == NoteGroup::Single) {
			throw std::runtime_error("ロング中のノートが指定されませんでした。");
		}

		while (true) {
			if(handler)handler(note[lane][index]);

			if (note[lane][index].group == NoteGroup::LongNoteStart) {
				return index;
			}
			index--;
			if (index < 0) {
				throw std::runtime_error("ロングノート始点が見つかりませんでした。");
			}
		}
	}

	/// <summary>
	/// laneとindexの位置(ロング中のみ)からロングノート終点のindexを返す
	/// </summary>
	int searchLnEnd(int lane, int ind, std::function<void(NOTE&)> handler = nullptr) {
		int index = ind;

		if (note[lane][index].group == NoteGroup::Single) {
			throw std::runtime_error("ロング中のノートが指定されませんでした。");
		}

		while (true) {
			if (handler)handler(note[lane][index]);

			if (note[lane][index].group == NoteGroup::LongNoteEnd) {
				return index;
			}
			index++;
			if (note[lane][index].group == NoteGroup::Single) {
				throw std::runtime_error("ロングノート終点が見つかりませんでした。");
			}
		}
	}
};


class BARLINE {//演奏画面で表示する小節線の構造体
public:
	float x = 0;//ノートのx座標
	float y = -128;//ノートのy座標
	int timing = 0;//表示用タイミング(ミリ秒)
	int timing_real = 0;//叩く実際のタイミング(ミリ秒)
	//char sound_number = 0;//鳴らす音の種類をビットで記入
	//int  sound_length = 0;//鳴らす音の長さ 
	char fall = 0;//0なら落ちてない,1なら落下中,2なら落ち切った
	float bpm = 1;//このノート時点のBPM
	BOOL use = 0;//使用フラグ
};

#endif