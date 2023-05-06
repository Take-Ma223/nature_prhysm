#pragma once
#include "STRUCT_Song.h"
#include <iostream>
#include <fstream>
#include <string>

/// <summary>
/// 譜面難易度算出クラス
/// </summary>
class AutoDifficultyPrediction
{
public:
	/// <summary>
	/// 難易度を取得
	/// </summary>
	/// <param name="Music">曲</param>
	/// <param name="difficulty">難易度</param>
	std::wstring getDifficulty(Song Music, int difficulty) {
		struct stat statBuf;
		wchar_t passbuf[512] = L"";

		wchar_t* command = {
		L"./programs/application/auto_difficulty_prediction/NPADP_pred.exe"
		};

		sprintfDx(passbuf, L"%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", command,
			Music.global[0][difficulty],
			Music.local[0][difficulty],
			Music.chain[0][difficulty],
			Music.unstability[0][difficulty],
			Music.longNote[0][difficulty],
			Music.color[0][difficulty],
			Music.ColorNotesAmount[difficulty][0],
			Music.ColorNotesAmount[difficulty][1],
			Music.ColorNotesAmount[difficulty][2],
			Music.ColorNotesAmount[difficulty][3],
			Music.ColorNotesAmount[difficulty][4],
			Music.ColorNotesAmount[difficulty][5],
			Music.ColorNotesAmount[difficulty][6],
			Music.ColorNotesAmount[difficulty][7],
			Music.ColorNotesAmount[difficulty][8],
			Music.LocalNotesAmount[difficulty][0],
			Music.LocalNotesAmount[difficulty][1],
			Music.LocalNotesAmount[difficulty][2],
			Music.LocalNotesAmount[difficulty][3],
			Music.LocalNotesAmount[difficulty][4],
			Music.LocalNotesAmount[difficulty][5],
			Music.LocalNotesAmount[difficulty][6],
			Music.LocalNotesAmount[difficulty][7],
			Music.LocalNotesAmount[difficulty][8]
			);

		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi;
		BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);//終了を待つ
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);

		std::wifstream ifs(L"save_data/tmp/auto_difficulty_prediction/pop.txt");
		std::wstring str;

		if (ifs.fail()) {
			return wstring(L"エラー ファイルが開けません");
		}

		getline(ifs, str);

		return str;

	}
};

