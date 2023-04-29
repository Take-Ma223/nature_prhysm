#pragma once
#include "STRUCT_NOTE.h"
#include "STRUCT_SEQ.h"
#include<functional>

using namespace std;

class DifficultyRadar {
public:
	DifficultyRadar(NOTE** note, int* nc, BPMC* bpmchange, STOP_SE* stopSequence, SC* scrollchange, int time, int StartTime, int EndTime, int* TimingSame, short* NotesAmount, double BPM_suggest);//イニシャライザ,各色の個数も算出
	int NumberTranslation(int color);//rgbycmwdfの番号をrgbcmywdfに変換
	void GetLocalNotesGraph(short* LocalNotesGraph);//局所降水量の9分割グラフを求める

	int CalcGlobal(int Rainbow);//降水量(平均)
	int CalcLocal(int Rainbow);//最大局所降水量
	int index(NOTE**, int*, int*);//CalcLocal用の現在のncから最も最初にある音符のindexを返す関数

	int CalcColor(int StartTime, int EndTime, int Rainbow);//色変化度 (算出開始時間~算出終了時間)

	//int CalcRange(int Rainbow);//同時押し度
	int CalcLongNote(int Rainbow);//LN率
	int CalcUnstability();//大気安定度(ソフラン度)
	int CalcChain();//縦連度

	void serachNotesBFS(function<void(int lane,int index)> handler);//音符の幅優先探索

	NOTE** note;
	int* nc;//音符個数
	BPMC* bpmchange;
	STOP_SE* stopSequence;
	SC* scrollchange;
	int time;
	int StartTime;
	int EndTime;
	int TotalNotes = 0;
	int TotalNotesRainbow = 0;
	int TotalNotesK = 0;
	int* TimingSame;

	int globalMax = 552;
	int localMax = 958;
	int colorMax = 350;
	int LNMax = 33;
	int unstabilityMax = 259;
	int chainMax = 120;
	double BPM_suggest = 0;

private:

};
