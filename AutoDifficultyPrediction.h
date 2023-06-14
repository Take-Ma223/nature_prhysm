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
	double getDifficulty(Song Music, int difficulty) {
		struct stat statBuf;
		std::string passbuf =
			std::to_string(Music.global[0][difficulty]) + "," +
			std::to_string(Music.local[0][difficulty]) + "," +
			std::to_string(Music.chain[0][difficulty]) + "," +
			std::to_string(Music.unstability[0][difficulty]) + "," +
			std::to_string(Music.longNote[0][difficulty]) + "," +
			std::to_string(Music.color[0][difficulty]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][0]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][1]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][2]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][3]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][4]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][5]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][6]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][7]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][8]) + "," +
			std::to_string(Music.LocalNotesAmount[difficulty][0]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][1]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][2]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][3]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][4]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][5]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][6]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][7]) + "," +
			std::to_string(Music.ColorNotesAmount[difficulty][8]);

		//STARTUPINFO si = { sizeof(STARTUPINFO) };
		//PROCESS_INFORMATION pi;
		//BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		////WaitForSingleObject(pi.hProcess, INFINITE);//終了を待つ
		//::CloseHandle(pi.hProcess);
		//::CloseHandle(pi.hThread);

		double difficultyValue = getDifficultyFromServer(passbuf);


		//std::wifstream ifs(L"save_data/tmp/auto_difficulty_prediction/pop.txt");
		//std::wstring str;

		//if (ifs.fail()) {
		//	return wstring(L"エラー ファイルが開けません");
		//}

		//getline(ifs, str);

		return difficultyValue;

	}

	static PROCESS_INFORMATION pi;
	static int dstSocket;

	/// <summary>
	/// AI難易度算出用サーバーを起動する
	/// </summary>
	void bootServer() {
		wchar_t command[] = L"./programs/application/auto_difficulty_prediction/NPADP_pred.exe";
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		BOOL CPres = ::CreateProcessW(NULL, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		//WaitForSingleObject(pi.hProcess, INFINITE);//終了を待つ
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);

		connectToServer();
	}

	void killServer() {
		/* Windows 独自の設定 */
		closesocket(dstSocket);
		WSACleanup();

		//UINT exitCode = 0;
		//TerminateProcess(pi.hProcess, exitCode);		
	}

	void connectToServer() {
		/* IP アドレス、ポート番号、ソケット */
		char* destination = "127.0.0.1";//localhost
		unsigned short port = 50001;
		

		/* sockaddr_in 構造体 */
		struct sockaddr_in dstAddr;

		/* 各種パラメータ */
		int status;
		int numsnt;
		char* toSendText = "This is a test";

		/************************************************************/

		/* Windows 独自の設定 */
		WSADATA data;
		WSAStartup(MAKEWORD(2, 0), &data);


		/* sockaddr_in 構造体のセット */
		memset(&dstAddr, 0, sizeof(dstAddr));
		dstAddr.sin_port = htons(port);
		dstAddr.sin_family = AF_INET;
		dstAddr.sin_addr.s_addr = inet_addr(destination);

		/* ソケット生成 */
		dstSocket = socket(AF_INET, SOCK_STREAM, 0);

		/* 接続 */
		//printf("Trying to connect to %s: \n", destination);
		connect(dstSocket, (struct sockaddr*)&dstAddr, sizeof(dstAddr));
	}

	double getDifficultyFromServer(string str) {
		static const int BUFFER_SIZE = 256;
		int numrcv;

		send(dstSocket, str.c_str(), strnlen_s(str.c_str(), 256) + 1, 0);

		char buffer[BUFFER_SIZE];
		int err = 0;

		//受信
		numrcv = recv(dstSocket, buffer, BUFFER_SIZE, 0);
		err = WSAGetLastError();
		if (numrcv == 0 || numrcv == -1) {
			int status = closesocket(dstSocket);
			return 0;
		}
		//printf("received: %s\n", buffer);

		return atof(buffer);
	}

};

