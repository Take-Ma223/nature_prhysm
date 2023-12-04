#pragma once
#include <fstream>
#include "STRUCT_Song.h"

using namespace std;

/// <summary>
/// 難易度算出ネットワーク学習用データ生成クラス
/// </summary>
class LearningDataGenarator
{
public:
	wofstream file;

	/// <summary>
	/// ファイルを開く
	/// </summary>
	void openFile() {
		file = wofstream(L"pred.csv");
		file << L"x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17,x18,x19,x20,x21,x22,x23,x24,y" << endl;
	}

	/// <summary>
	/// 1譜面書き込む
	/// </summary>
	void writeData(Song* Music, int difficulty) {
		file << Music->global[0][difficulty] << L",";
		file << Music->local[0][difficulty] << L",";
		file << Music->chain[0][difficulty] << L",";
		file << Music->unstability[0][difficulty] << L",";
		file << Music->longNote[0][difficulty] << L",";
		file << Music->color[0][difficulty] << L",";
		file << Music->ColorNotesAmount[RainbowMode::General][difficulty][0] << L",";
		file << Music->ColorNotesAmount[RainbowMode::General][difficulty][1] << L",";
		file << Music->ColorNotesAmount[RainbowMode::General][difficulty][2] << L",";
		file << Music->ColorNotesAmount[RainbowMode::General][difficulty][3] << L",";
		file << Music->ColorNotesAmount[RainbowMode::General][difficulty][4] << L",";
		file << Music->ColorNotesAmount[RainbowMode::General][difficulty][5] << L",";
		file << Music->ColorNotesAmount[RainbowMode::General][difficulty][6] << L",";
		file << Music->ColorNotesAmount[RainbowMode::General][difficulty][7] << L",";
		file << Music->ColorNotesAmount[RainbowMode::General][difficulty][8] << L",";
		file << Music->LocalNotesAmount[RainbowMode::General][difficulty][0] << L",";
		file << Music->LocalNotesAmount[RainbowMode::General][difficulty][1] << L",";
		file << Music->LocalNotesAmount[RainbowMode::General][difficulty][2] << L",";
		file << Music->LocalNotesAmount[RainbowMode::General][difficulty][3] << L",";
		file << Music->LocalNotesAmount[RainbowMode::General][difficulty][4] << L",";
		file << Music->LocalNotesAmount[RainbowMode::General][difficulty][5] << L",";
		file << Music->LocalNotesAmount[RainbowMode::General][difficulty][6] << L",";
		file << Music->LocalNotesAmount[RainbowMode::General][difficulty][7] << L",";
		file << Music->LocalNotesAmount[RainbowMode::General][difficulty][8] << L",";
		file << Music->level[difficulty] << endl;
	}












};

