#pragma once
#include "STRUCT_NOTE.h"
#include "STRUCT_SEQ.h"
#include<functional>

using namespace std;

class DifficultyRadar {
public:
	DifficultyRadar(NOTE** note, int* nc, BPMC* bpmchange, STOP_SE* stopSequence, SC* scrollchange, int time, int StartTime, int EndTime, int* TimingSame, short* NotesAmount, double BPM_suggest);//�C�j�V�����C�U,�e�F�̌����Z�o
	int NumberTranslation(int color);//rgbycmwdf�̔ԍ���rgbcmywdf�ɕϊ�
	void GetLocalNotesGraph(short* LocalNotesGraph);//�Ǐ��~���ʂ�9�����O���t�����߂�

	int CalcGlobal(int Rainbow);//�~����(����)
	int CalcLocal(int Rainbow);//�ő�Ǐ��~����
	int index(NOTE**, int*, int*);//CalcLocal�p�̌��݂�nc����ł��ŏ��ɂ��鉹����index��Ԃ��֐�

	int CalcColor(int StartTime, int EndTime, int Rainbow);//�F�ω��x (�Z�o�J�n����~�Z�o�I������)

	//int CalcRange(int Rainbow);//���������x
	int CalcLongNote(int Rainbow);//LN��
	int CalcUnstability();//��C����x(�\�t�����x)
	int CalcChain();//�c�A�x

	void serachNotesBFS(function<void(int lane,int index)> handler);//�����̕��D��T��

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
	int unstabilityMax = 259;
	int chainMax = 120;
	double BPM_suggest = 0;

private:

};
