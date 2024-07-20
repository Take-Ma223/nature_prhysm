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
		std::shared_ptr<std::vector<float>> bpmList);//�C�j�V�����C�U,�e�F�̌����Z�o
	int NumberTranslation(NoteColor color);//rgbycmwdf�̔ԍ���rgbcmywdf�ɕϊ�
	void GetLocalNotesGraph(short* LocalNotesGraph, bool isRainbow);//�Ǐ��~���ʂ�9�����O���t�����߂�
	void GetColorNotesGraph(short* ColorNotesGraph, short* ColorNotesGraphR);//�F�O���t�����߂�

	int CalcGlobal(bool isRainbow);//�~����(����)
	int CalcLocal(bool isRainbow);//�ő�Ǐ��~����
	int index(NOTE**, int*, int*);//CalcLocal�p�̌��݂�nc����ł��ŏ��ɂ��鉹����index��Ԃ��֐�

	int CalcColor(int StartTime, int EndTime, bool isRainbow);//�F�ω��x (�Z�o�J�n����~�Z�o�I������)

	//int CalcRange(int Rainbow);//���������x
	int CalcLongNote(bool isRainbow);//LN��
	int CalcUnstability();//��C����x(�\�t�����x)
	int CalcChain();//�c�A�x

	void serachNotesFromEarly(function<void(int lane,int index)> handler);//�����̕��D��T��

	NOTE** note;
	int* nc;//������
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
