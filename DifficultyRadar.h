#pragma once
#include "STRUCT_NOTE.h"
#include "STRUCT_SEQ.h"
#include<functional>
#include <memory>

using namespace std;

class DifficultyRadar {
public:
	DifficultyRadar(
		NOTE** note, 
		int* nc, 
		BPMC* bpmchange, 
		STOP_SE* stopSequence,
		SC* scrollchange, 
		int time, 
		int StartTime, 
		int EndTime, 
		int* TimingSame, 
		double BPM_suggest, 
		std::shared_ptr<std::vector<float>> bpmList);//イニシャライザ,各色の個数も算出
	int NumberTranslation(NoteColor color);//rgbycmwdfの番号をrgbcmywdfに変換
	void GetLocalNotesGraph(short* LocalNotesGraph, bool isRainbow);//局所降水量の9分割グラフを求める
	void GetColorNotesGraph(short* ColorNotesGraph, short* ColorNotesGraphR);//色グラフを求める

	int CalcGlobal(bool isRainbow);//降水量(平均)
	int CalcLocal(bool isRainbow);//最大局所降水量
	int index(NOTE**, int*, int*);//CalcLocal用の現在のncから最も最初にある音符のindexを返す関数

	int CalcColor(int StartTime, int EndTime, bool isRainbow);//色変化度 (算出開始時間~算出終了時間)

	//int CalcRange(int Rainbow);//同時押し度
	int CalcLongNote(bool isRainbow);//LN率
	int CalcUnstability();//大気安定度(ソフラン度)
	int CalcChain();//縦連度

	void serachNotesFromEarly(function<void(int lane,int index)> handler);//音符の幅優先探索

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
	int unstabilityMax = 290;
	int chainMax = 120;
	double BPM_suggest = 0;
	std::shared_ptr<std::vector<float>> bpmList;
private:

};
